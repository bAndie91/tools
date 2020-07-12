#!/usr/bin/env phantomjs2 --ssl-protocol=any --ignore-ssl-errors=true --web-security=false

/*
	EXIT CODE
	0	report successful
	1	a web interaction step failed on user side
	2	wrong invocation
	3	page load failed
	4	a web interaction step failed on navigator side
	5	report failed
*/

var llDebug = 1, llVerbose = 2, llError = 3, llMsg = 4;
var vlDebug = false;
var vlVerbose = false;
var vlError = true;
var vlMsg = true;
var system = require('system');
var stderr = system.stderr;
var stdout = system.stdout;
var Glob = {};

for(var i=1; i<system.args.length; i++)
{
	if(system.args[i] == '--debug') vlDebug = true;
	else if(system.args[i] == '--verbose') vlVerbose = true;
	else if(system.args[i].match(/^-/))
	{
		stderr.write("Unknown option: "+system.args[i]+"\n");
		phantom.exit(2);
	}
	else
	{
		if(!Glob.telepules)
		{
			Glob.telepules = system.args[i];
		}
		else if(!Glob.date_from)
		{
			Glob.date_from = system.args[i];
		}
		else if(!Glob.date_till)
		{
			Glob.date_till = system.args[i];
		}
	}
}

if(!Glob.telepules || !Glob.date_from || !Glob.date_till)
{
	stderr.write("Usage: nkm-aramszunet.js [--debug] [--verbose] <település> <dátum-tól> <dátum-ig>\n");
	phantom.exit(2);
}


var url_form = "https://www.nkmenergia.hu/aram/pages/online/aramszunet.jsf";
var loadInProgress = false;
var stepindex = 0;
var page = new WebPage();
page.settings.userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.90 Safari/537.36 Vivaldi/1.91.867.38";

function log(level, msg)
{
	if(level == llDebug && vlDebug || 
	   level == llVerbose && vlVerbose ||
	   level == llError && vlError ||
	   level == llMsg && vlMsg)
	{
		stderr.write(msg + "\n");
	}
}

page.onConsoleMessage = function(msg)
{
	if(msg.match(/^\[DEBUG\]/))
		log(llDebug, msg);
	else if(msg.match(/^\[ERROR\]/))
		log(llError, msg);
	else
		log(llMsg, ">>> " + msg);
};

page.onLoadStarted = function()
{
	loadInProgress = true;
	log(llVerbose, "load started [" + page.url + "]");
};

page.onLoadFinished = function()
{
	loadInProgress = false;
	log(llVerbose, "load finished <" + page.evaluate(function(){return document.title;}) + "> [" + page.url + "]");
};

page.onError = function(msg, stack)
{
	log(llError, "ERROR: " + msg + "\nSTACK: " + JSON.stringify(stack));
};

page.onResourceRequested = function(requestData, networkRequest)
{
	/* allow only these domains, no trackers, no ads */
	var allow = false;
	var g = requestData.url.match(/^.+?:\/\/(.+?)(\/[^\?]*)/);
	var host = g[1];
	var path = g[2];
	var hostpath = host+path;
	// google maps seem to needed for the page to work properly, but I worked it around
	//if(hostpath == 'maps.google.com/maps/api/js') allow = true;
	if(host == 'www.nkmenergia.hu') allow = true;
	if(!allow)
	{
		log(llDebug, "deny: " + requestData.method + " " + requestData.url);
		networkRequest.abort();
	}
	
	if(requestData.url.match(/^https?:\/\/(maps\.google\.com)\//))
	{
		// mock out some base parts of gmaps lib
		page.evaluate(function(){
			window.google = {};
			google.maps = {};
			google.maps.MapTypeId = {};
			google.maps.MapTypeId.ROADMAP = {};
			google.maps.LatLng = function(){ return {}; };
			google.maps.InfoWindow = function(){ return {}; };
			google.maps.Map = function(){ return {
				'getBounds': function(){ return {}; },
			}; };
			google.maps.event = { 'addListener': function(){ return {}; }, };
			google.maps.Marker = function(){ return {
				'setMap': function(){ return {}; },
			}; };
			console.log("gmaps mocked.");
		});
	}
};


Glob.inject = 'function click(elem){'+
'	var ev = document.createEvent("MouseEvent");'+
'	ev.initMouseEvent("click", true /* bubble */, true /* cancelable */, window, null, 0, 0, 0, 0, /* coordinates */ false, false, false, false, /* modifier keys */ 0 /*left*/, null);'+
'	elem.dispatchEvent(ev);'+
'};'+
'function grab_output(){'+
'	return document.querySelector(\'[id="aramszunetForm:aramszunetTable_data"]\').innerHTML;'+
'};'+
'function grab_aramszunetTable(){'+
'	return document.querySelector(\'[id="aramszunetForm:aramszunetTable"]\').innerHTML;'+
'};';



page.open(url_form, function(status)
{
	if(status !== 'success')
	{
		log(llError, "ERROR " + status);
		phantom.exit(3);
	}
	else
	{
		
		var steps = [
			function(param)
			{
				/* Waiting page to load */
				var input_telepules = document.querySelector('select[id="aramszunetForm:telepules_input"]');
				if(!input_telepules)
				{
					//console.log(document.querySelectorAll('html')[0].outerHTML);
					console.log("Waiting for 'Település' field...");
					return 0;
				}
				return 1;
			},
			function(param)
			{
				// save the empty 'no results' table for comparasion later on
				eval(param.inject);
				var noResults = grab_aramszunetTable();
				var rv = {'result': {'aszt': noResults}, 'jump': 1};
				return JSON.stringify(rv);
			},
			function(param)
			{
				console.log("submitting form...");
				var telepules_element = document.querySelector('li[data-label="' + param.telepules + '"]');
				if(!telepules_element)
				{
					var msg = "crawler: A település '" + param.telepules + "' nincs a tervezett áramszünetek listájában."
					console.log(msg);
					var output = "<tr><td>"+msg+"</td></tr>";
					return JSON.stringify({'result': {'output': output}, 'jump': 'end'});
				}
				eval(param.inject);
				/* Filling in form fields and submit */
				click(telepules_element);
				document.querySelector('input[id="aramszunetForm:startDate_input"]').value = param.date_from;
				document.querySelector('input[id="aramszunetForm:endDate_input"]').value = param.date_till;
				
				// 3rd party components are celled via this method,
				// ignore errors in 3rd parties (errors occur due to not loaded 3rd party scripts)
				PrimeFaces.ajax.ResponseProcessor.doEval = function(b){
					var a = b.textContent || b.innerText || b.text;
					try { $.globalEval(a) }
					catch(e) { console.log('Exception', e); }
				};
				// the onClick event handler on the submit button looks like this:
				//PrimeFaces.ab({s:"aramszunetForm:j_idt29",p:"aramszunetForm",u:"aramszunetForm:aramszunetTable aramszunetForm:filters"});
				
				var submit_btn = document.querySelector('#aramszunetForm button');
				click(submit_btn);
				return 1;
			},
			function(param)
			{
				// there were many trial-and-error here to find out how to set rows-per-page which PrimeFaces understands
				console.log("setting paginator rows per page to 1000...");
				var rppdd = document.querySelector('select[name="aramszunetForm:aramszunetTable_rppDD"]');
				var opt = document.createElement('option');
				opt.value = 1000;
				opt.innerText = "1000";
				rppdd.appendChild(opt);
				//rppdd.selectedIndex = rppdd.options.length - 1;
				//opt.setAttribute('selected', 'selected');
				rppdd.value = opt.value;
				//rppdd.setAttribute('value', opt.value);
				//$(rppdd).val(opt.value);
				PrimeFaces.widgets.aramszunetTable.paginator.setRowsPerPage(opt.value);
				//console.log("rppdd=", $(rppdd).val(), rppdd.value, rppdd.getAttribute('value'));
				//opt.selected = true;
				//$(rppdd).val(opt.value);$(rppdd).change();
				
				//var ev = document.createEvent("HTMLEvents");
				//ev.initEvent('change', true /* bubble */, true /* cancelable */);
				//rppdd.dispatchEvent(ev);
				return 1;
			},
			function(param)
			{
				eval(param.inject);
				console.log("waiting 'aramszunetTable' to change...");
				var aszt = grab_aramszunetTable();
				var rv = {'result': {'aszt': aszt}, 'jump': 0};
				if(param.aszt != rv.result.aszt) {
					rv.jump = 1;
				}
				return JSON.stringify(rv);
			},
			function(param)
			{
				eval(param.inject);
				var rv = {'result': {'output': grab_output()}, 'jump': 1};
				return JSON.stringify(rv);
			}
		];
		
		Glob.total_steps = steps.length;
		
		timeout = 500;
		worker = function()
		{
			if(!loadInProgress)
			{
				/* Execute next step in navigator's context */
				log(llDebug, "step " + stepindex);
				var ret = page.evaluate(steps[stepindex], Glob);
				
				if(ret === '')
				{
					phantom.exit(1);
				}
				else if(ret === undefined || ret === null)
				{
					phantom.exit(4);
				}
				
				if(typeof ret != "number")
				{
					ret = JSON.parse(ret);
					for(var key in ret.result)
					{
						Glob[key] = ret.result[key];
					}
					ret = ret.jump;
				}
				
				log(llDebug, "step " + stepindex + " jump " + ret);
				if(ret == 'end') stepindex = steps.length;
				else stepindex += ret;
				
				if(stepindex >= steps.length)
				{
					stdout.write(Glob.output);
					log(llDebug, "completed");
					phantom.exit(0);
				}
			}
			
			setTimeout(worker, timeout);
		}		
		setTimeout(worker, timeout);
	}
});
