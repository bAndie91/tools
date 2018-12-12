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
var llDebug = 1, llStatus = 2, llError = 3, llMsg = 4;
var vlDebug = false;
var vlStatus = false;
var vlError = true;
var vlMsg = true;
var system = require('system');
var stderr = system.stderr;
var stdout = system.stdout;
var Glob = {};

for(var i=1; i<system.args.length; i++)
{
	if(system.args[i] == '--debug') vlDebug = true;
	else if(system.args[i] == '--verbose') vlStatus = true;
	else if(system.args[i].match(/^-/))
	{
		stderr.write("Unknown option: "+system.args[i]+"\n");
		phantom.exit(2);
	}
	else
	{
		Glob.felhasznalo = system.args[i];
		Glob.jelszo = system.args[i+1];
		i += 2;
	}
}

//TODO paramater handling
//if(Glob.datum == undefined)
//{
//	stderr.write("Usage: nkmfoldgaz.js [--debug] [--verbose] <felhasználó-azonosító> <mérőóra-gyári-szám> <mérőállás> <email> <dátum>\n");
//	phantom.exit(2);
//}



//var url_form = "https://www.nkmaram.hu/pages/wrapper.jsp?id=740&ebillCmd=login";
var url_form = "https://www.nkmaram.hu/pages/wrapper.jsp?ebillCmd=login";
Glob.url_history = "https://www.nkmaram.hu/pages/online/korabbiMeroAllOnlineUsz.jsf?id=1300460";
Glob.url_report = "https://www.nkmaram.hu/meroallas/?id=1300461";
var loadInProgress = false;
var stepindex = 0;
var page = new WebPage();
page.settings.userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.90 Safari/537.36 Vivaldi/1.91.867.38";

function log(level, msg)
{
	if(level == llDebug && vlDebug || 
	   level == llStatus && vlStatus ||
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
	else
		log(llMsg, ">>> " + msg);
};

page.onLoadStarted = function()
{
	loadInProgress = true;
	log(llStatus, "load started [" + page.url + "]");
};

page.onLoadFinished = function()
{
	loadInProgress = false;
	log(llStatus, "load finished <" + page.evaluate(function(){return document.title;}) + "> [" + page.url + "]");
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
				var form = document.querySelector('form#meroTajForm');
				if(!form)
				{
					console.log("Waiting for page "+location.href+" ...");
					return 0;
				}
				return 1;
			},
			function(param)
			{
				// TODO submit form to proceed
				return false;
			}
		];

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
					log(llDebug, "complete");
					if(Glob.result && Glob.result.match(/POSITIVE/))
					{
						stderr.write("[OK] Transaction passed.\n");
						stdout.write(Glob.result);
						phantom.exit(0);
					}
					else
					{
						stderr.write("[ERROR] Transaction failed.\n");
						stderr.write(Glob.result_elem_html_outer + "\n");
						stderr.write(Glob.result_elem_text_inner + "\n");
						stderr.write(Glob.result + "\n");
						phantom.exit(5);
					}
				}
			}
			
			setTimeout(worker, timeout);
		}		
		setTimeout(worker, timeout);
	}
});
