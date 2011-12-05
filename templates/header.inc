<? assert('!isset($tags) || is_array($tags)'); ?><!DOCTYPE html>
<html lang="en">
<? global $preview; ?>
<head>
	<meta charset="utf-8">
	<title><? if(function_exists("title")) { echo title(); } else { echo "the emperor's new title"; } ?></title>
  <link rel="stylesheet" href="<?= WEBROOT ?>styles/main.css" type="text/css" media="screen" title="no title" charset="utf-8" />
  <link rel="stylesheet" href="<?= WEBROOT ?>styles/code.css" type="text/css" media="screen" title="no title" charset="utf-8" />
  <meta id="iphone-viewport" content="width=620 maximum-scale=0.6667" name="viewport"/>
	<script type="text/javascript" src="<?= WEBROOT ?>js/jquery-1.7.1.min.js"></script>
	<script type="text/javascript">
		var jsScripts = ["<?= WEBROOT ?>js/helpers.js"];
		var jsLoaded = [];
		var jsLocked = false;
		
		// http://code.google.com/speed/page-speed/docs/payload.html#DeferLoadingJS
		// Add a script element as a child of the body
		function jsInsert() {
			if(jsScripts.length == 0) {
				return;
			}

			if(jsLoaded.length == 0) {
				while(script = jsScripts.shift()) {
					var element = document.createElement("script");
					element.charset = "utf-8";
					element.type = "text/javascript";
				  element.src = script;
				  document.body.appendChild(element);
					console.log(script);
					jsLoaded.push(script);
				}
			} else {
				if(jsLocked) {
					return;
				}
				jsLocked = jsScripts.shift();

				$.ajax({
					type: "GET",
					url: jsLocked,
					success: function(data) {
						// NOTE: script is automatically evaluated by jQuery
						console.log(jsLocked);
						jsLoaded.push(jsLocked);

						// unlock and handle the next script
						jsLocked = false;
						jsInsert();
					},
					dataType: "script"
				});
			}
		}

		function jsLoad(script) {
			// duplicate barrier
			if(jsScripts.indexOf(script) == -1
			&& jsLoaded.indexOf(script) == -1)
			{
				jsScripts.push(script);
			}

			// avoid pre-empting the document's ready handler
			if(jsLoaded.length) {
				jsInsert();
			}
		}

		// when the document is loaded, insert pending js
	 	if (window.addEventListener) {
	 		window.addEventListener("load", jsInsert, false);
	 	} else if (window.attachEvent) {
	 		window.attachEvent("onload", jsInsert);
	 	} else {
			window.onload = jsInsert;
		}
	</script>
	<link rel="icon" type="image/png" href="<?= WEBROOT ?>favicon.ico">
	<? if(function_exists("head")) { echo head(); } ?>
</head>
<body>
	<div id="container"<? if(function_exists("postwidth")) echo ' style="width:'.(postwidth() + 43).'px;"'; ?>>
		<? if(function_exists("nav")) { echo nav(); } ?>
  		<div id="entries">