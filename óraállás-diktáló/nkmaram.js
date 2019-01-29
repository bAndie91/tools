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

//TODO factor out
var llDebug = 1, llVerbose = 2, llError = 3, llMsg = 4;
var vlDebug = false;
var vlVerbose = false;
var vlError = true;
var vlMsg = true;
var system = require('system');
var stderr = system.stderr;
var stdout = system.stdout;
var Glob = {
	'show_history_only': false,
};

for(var i=1; i<system.args.length; i++)
{
	if(system.args[i] == '--debug') vlDebug = true;
	else if(system.args[i] == '--verbose') vlVerbose = true;
	else if(system.args[i] == '--history') Glob.show_history_only = true;
	else if(system.args[i].match(/^-/))
	{
		stderr.write("Unknown option: "+system.args[i]+"\n");
		phantom.exit(2);
	}
	else
	{
		if(!Glob.felhasznalo)
		{
			Glob.felhasznalo = system.args[i];
		}
		else if(!Glob.jelszo)
		{
			Glob.jelszo = system.args[i];
		}
		else
		{
			if(system.args[i].match(/^[0-9]+$/))
			{
				if(typeof Glob.oraallas == "undefined") Glob.oraallas = [];
				Glob.oraallas.push(system.args[i]);
			}
			else
			{
				stderr.write("Invalid óraállás: "+system.args[i]+"\n");
				phantom.exit(2);
			}
		}
	}
}

if(!Glob.oraallas && !Glob.show_history_only)
{
	stderr.write("Usage: nkmaram.js [--debug] [--verbose] <felhasználó-azonosító> <jelszó> [--history | <mérőállás-1> [<mérőállás-2> ...]]\n");
	phantom.exit(2);
}



var url_form = "https://www.nkmaram.hu/pages/wrapper.jsp?ebillCmd=login" /* ?id=740&ebillCmd=login */;
Glob.url_history = "https://www.nkmaram.hu/pages/online/korabbiMeroAllOnlineUsz.jsf?id=1300460";
Glob.url_report = "https://www.nkmaram.hu/meroallas/?id=1300461";
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
	if(!requestData.url.match(/^data:/))
	{
		log(llDebug, requestData.method+' '+requestData.url);
	}
	/*
	for(var i=0; i<requestData.headers.length; i++)
	{
		log(llDebug, requestData.headers[i].name + ': ' + requestData.headers[i].value);
	}
	log(llDebug, JSON.stringify(requestData));
	*/
};


/* Define common functions used in webpage */
Glob.clickFunc = '(function(elem)'+
'{'+
'	var ev = document.createEvent("MouseEvent");'+
'	ev.initMouseEvent("click", true /* bubble */, true /* cancelable */, window, null, 0, 0, 0, 0, /* coordinates */ false, false, false, false, /* modifier keys */ 0 /*left*/, null);'+
'	elem.dispatchEvent(ev);'+
'})';


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
				var field_user = document.querySelector('input.InputText[name=user]');
				if(!field_user)
				{
					//console.log(document.querySelectorAll('html')[0].outerHTML);
					console.log("Waiting for login field...");
					return 0;
				}
				return 1;
			},
			function(param)
			{
				/* Filling in form fields and submit */
				
				var field_user = document.querySelector('input.InputText[name=user]');
				var field_pass = document.querySelector('input.InputText[name=password]');
				
				field_user.value = param.felhasznalo;
				field_pass.value = param.jelszo;
				
				var form = document.querySelector('.userContainer form.loginForm');
				form.submit();
				
				return 1;
			},
			function(param)
			{
				var messageBox = document.querySelector('.messageBox');
				var egyenleg_elem = document.querySelector('.szsz_egyenleg');
				if(!egyenleg_elem && !messageBox)
				{
					console.log("Waiting for page to load...");
					return 0;
				}
				if(messageBox)
				{
					console.log(messageBox.innerText);
					return false;
				}
				var egyenleg = egyenleg_elem.innerText;
				console.log("Egyenleg: " + egyenleg);
				param.egyenleg = egyenleg;
				return JSON.stringify({
					jump: 1,
					result: param,
				});
			},
			function(param)
			{
				var table = document.querySelector('form[name=korabbiMeroAllFormOnlineusz] table.rtable');
				if(!table)
				{
					if(location.href == param.url_history)
						console.log("Waiting for page to load...");
					else
						location.href = param.url_history;
					return 0;
				}
				console.log(table.innerText);
				if(param.show_history_only) return param.total_steps + 1;
				return 1;
			},
			function(param)
			{
				/* Navigate to 'Mérőállás bejelentés' page */
				location.href = param.url_report;
				return 1;
			},
			function(param)
			{
				/* Wait for page to load */
				var buttons = document.querySelectorAll('form#meroTajForm input.button');
				if(!buttons)
				{
					console.log("Waiting for page "+location.href+" ...");
					return 0;
				}
				for(var i = 0; i<buttons.length; i++)
				{
					console.log("[DEBUG] button \""+buttons[i].value+"\"");
					if(buttons[i].value.match(/bejelent/))
					{
						/* Submit form to proceed (1) */
						eval(param.clickFunc)(buttons[i]);
						return 1;
					}
				}
				return 0;
			},
			function(param)
			{
				/* Exit if we got an error page */
				if(location.pathname.match(/meroHibalap/))
				{
					console.log(document.querySelector('form#meroHibalapForm').innerText);
					return false;
				}
				/* Wait for page to load */
				var buttons = document.querySelectorAll('form#meroTajRegForm input.button');
				if(!buttons)
				{
					console.log("Waiting for page "+location.href+" ...");
					return 0;
				}
				for(var i = 0; i<buttons.length; i++)
				{
					console.log("[DEBUG] button \""+buttons[i].value+"\"");
					if(buttons[i].value.match(/folytat/))
					{
						/* Submit form to proceed (2) */
						eval(param.clickFunc)(buttons[i]);
						return 1;
					}
				}
				return 0;
			},
			function(param)
			{
				var submit_btn = document.querySelector('form#meroallasBejelentes input.button');
				if(!submit_btn)
				{
					console.log("Waiting for page "+location.href+" ...");
					return 0;
				}
				
				var inputs = document.querySelectorAll('form#meroallasBejelentes input.numberFormat');
				for(var i = 0; i < inputs.length; i++)
				{
					console.log("Enter '"+param.oraallas[i]+"'");
					inputs[i].value = param.oraallas[i];
				}
				
				/* Sumbit Utility Usage Report */
				eval(param.clickFunc)(submit_btn);
				
				return 1;
			},
			function(param)
			{
				var summary = document.querySelector('form#szamlaForm');
				if(!summary)
				{
					console.log("Waiting for page "+location.href+" ...");
					return 0;
				}
				console.log(summary.innerText);
				
				/* Accept Utility Bill */
				var accept_btn = document.querySelector('input[name="szamlaForm:bOK"]');
				eval(param.clickFunc)(accept_btn);
				
				return 1;
			},
			function(param)
			{
				/* Wait for page to load */
				var readyForm = document.querySelector('form#readyForm');
				if(!readyForm)
				{
					console.log("Waiting for page "+location.href+" ...");
					return 0;
				}
				/* Display Service Provider's message*/
				console.log(readyForm.innerText);
				return 1;
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
				
				if(ret === false)
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
					for(var key in Glob)
					{
						if(!ret.result.hasOwnProperty(key)) delete Glob[key];
					}
					for(var key in ret.result)
					{
						Glob[key] = ret.result[key];
					}
					//stderr.write(JSON.stringify(Glob));
					ret = ret.jump;
				}
				
				log(llDebug, "step " + stepindex + " jump " + ret);
				stepindex += ret;
				
				if(stepindex >= steps.length)
				{
					log(llDebug, "completed");
					phantom.exit(0);
				}
			}
			
			setTimeout(worker, timeout);
		}		
		setTimeout(worker, timeout);
	}
});
