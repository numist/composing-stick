// http://tumbledry.org/2011/05/12/screw_hashbangs_building

// (4)
var pInfScrStates = {
	idle: 0,
	loading: 1,
	success: 2
}
// pInfScr = pax infinite scroll
var pInfScrState = pInfScrStates.idle;

// turn on or off intra-page navigation when back button used
// 1 = back button navigates WITHIN infinitely scrolling page
// 0 = back button navigates to previous, referring page
var pInfScrIntraPageBackButton = 0;

/* 
Pax browsing supports the following:
(1) 	/category/page/3/ loads the third page of results from that category
(2) 	/category/pages/3/ loads 3 pages of results, including page 3, from the beginning
			/category/pages/1-3/ does the same as previous, except page 1 is explicit
			/category/pages/2-3/ loads 2 pages of results, including page 3, excluding page 1

Why is this necessary?

We don't want to break the back button. 
(1) 	User loads /all/page/1/
(2) 	Scrolling dynamically adds the contents of /all/page/2/ to the page
 			At the same time, the history is updated to reflect the state of the page.
			That's where the pageS capability comes in.
(3a)	As long as we started on page 1, we can just update history to /all/pages/2/
			That way, when the user hits the back button, /all/pages/2/ will be loaded.
(3b)	What if the user started at page /all/page/2/ ?
			The user scrolls, and the page dynamically updates with contents of /all/page/3/
			The history is updated to reflect the state of the page.
			PROBLEM! The history no longer reflects the state of the page.
			The history says the current page is /all/pages/3/, but we started browsing at /all/page/2/.
			You see where this is going?
			This is a problem, because when the user hits the back button, they'll end up at an unfamiliar place
			on the resulting page because /all/pages/3/ loads 3 pages, including page 1. But we started at page 2.
(4)		Instead of simply asking for the number of pages loaded, we also explicitly request the start of page range.
			So, in (3b), user begins at /all/page/2/
			Scrolling results in dynamic load of /all/page/3/, and history is updated to reflect the state:
			History becomes /all/pages/2-3/.
			User scrolls more, resulting in dynamic load of /all/page/4/, history is updated to reflect the state:
			History becomes /all/pages/2-4/.

That unchanging value (2 in the example above) is the page base we must hang onto through 
dynamic loads and history state updates.
Hence the variable below.
Phew.
*/
var pInfScrUrlCarryPageBase = null;

// we'd also like to know the number of items returned
// that way, we can clear them out if the user hits the 'back' button
var pInfScrCount = new Array();

// we'll dynamically change the delay on the loading function,
// depending on the situation
// (1) DEFAULT: user scrolls down to trigger zone, 200 milliseconds, event fires
// (2) SPECIAL: user hits back button, inadvertently in target zone, 5000 millisecond delay, giving user a chance to scroll away from trigger zone
// delay, in milliseconds, for firing debounced infinite scroll function
var pInfScrDelay = 200;

function pInfScrExecute() {
	// jQuery plugin pathchange was authored by Ben Cherry (bcherry@gmail.com),
	// and is released under an MIT License (do what you want with it).
	// http://www.adequatelygood.com/2010/7/Saner-HTML5-History-Management
	// http://www.bcherry.net/playground/sanerhtml5history
	// http://www.bcherry.net/static/lib/js/jquery.pathchange.js
	// Some of the code in this plugin was adapted from Modernizr, which is also available under an MIT License.
	//
	// Modified by Alex Micek to change from a web-app type plugin (intercepting all links, stopping all hashes),
	// to an on-demand function for managing infinite scroll.
	// If path change requested, it is only done in browsers where we can avoid breaking the back button.
	// Contrary to Cherry's original authoring of the plugin, I chose NOT to use hash functionality
	// (monitoring, changing) to preserve application state. Instead, we simply say we cannot
	// do a pathchange, and the calling script takes appropriate action.
	
	// Simple feature detection for History Management (borrowed from Modernizr)
	function detectHistorySupport() {
		return !!(window.history && history.pushState);
	}

	function pathchange(path) {	
		// remember: pushState/replaceState only allows same-origin changes
		// https://developer.mozilla.org/en/DOM/Manipulating_the_browser_history#The_pushState().c2.a0method
    
		// as explicated in the "Pax browsing" comment above, we need a base page number
		// set ONLY ONCE per complete page load
		if(pInfScrUrlCarryPageBase == null) {
			/* path will be checked for in two places
			(1) browser URL, where it looks like this: 
					/category/page/xx/
					or this:
					/category/pages/xx-yy/
			(2) the link to the next page:
					/category/page/zz/ in the path variable
			
			xx is the number we are looking for (pInfScrUrlCarryPageBase)
			yy is a number greater than xx
			zz is xx+1
			
			Naturally, we put precedence on version (1); it's less brittle
			*/
    
			// instanatiate a regular expression that will match the following
			// /page/ or /pages/
			// 1+ 		numbers
			// 0 or 1 dash
			// 0+ 		numbers
			// 0 or 1 forward slash
			// tacked to the end of the string 
			regexp = /\/pages?\/([0-9]+)-?[0-9]*\/?$/;
			
			// (1)
			if(regexp.test(window.location.href)) {
				nextPage = regexp.exec(window.location.href);
				pInfScrUrlCarryPageBase = nextPage[1];
			}
			// (2)
			else {
				nextPage = regexp.exec(path);
				// so xx - 1 will give us the page base				
				pInfScrUrlCarryPageBase = nextPage[1] - 1;
			}				
		}
    
		// modify path with xx
		path = path.replace("page/","pages/"+pInfScrUrlCarryPageBase+"-");
		
		// change state and report that it worked
		if(pInfScrIntraPageBackButton == 1)
			window.history.pushState(null, null, path);
		else
			window.history.replaceState(null, null, path);
	}

	// doc.height()	unitless pixel value of HTML document height
	// .scrollTop()	pixels hidden from view above scrollable area
	//							0 if scroll bar at top, or element not scrollable
	// win.height()	unitless pixel value of browser viewport height
	
	// an AJAX request for content to append to the current page is made if:
	// (1) scroll to 200px or less from bottom of document
	// (2) not currently in process of loading new content
	if($(document).height() - 200 < $(document).scrollTop() + $(window).height() && pInfScrState == pInfScrStates.idle) {
	
		// get URL
		pInfScrNode = $('article#more').last();
		pInfScrURL = $('article#more p a').last().attr("href");

		// make request if
		// (3) node was found
		// (3.5) node is not hidden
		// (4) pathchange is supported, to preserve back button functionality (SCREW THE HASH BANG OPTION)
		if(pInfScrNode.length > 0 && pInfScrNode.css('display') != 'none' && detectHistorySupport()) {
		
			// node was found, pathchange worked... make request
			$.ajax({
				type: 'GET',
				url: pInfScrURL,
				beforeSend: function() {
					// block potentially concurrent requests
					pInfScrState = pInfScrStates.loading;
					
					// display loading feedback
					pInfScrNode.clone().empty().insertAfter(pInfScrNode).append('<p>loading…</p>');

					// hide 'more' browser
					pInfScrNode.hide();
				},
				success: function(data) {
					// use nodetype to grab elements
					var filteredData = $(data).find("article");

					if(filteredData.length > 0) {
						// no errors!
						pInfScrState = pInfScrStates.success;
						
						// count the number of items returned
						pInfScrCount.push(filteredData.length);
					
						// drop data into document
						filteredData.insertAfter(pInfScrNode);
						
						// update the address bar
						pathchange(pInfScrURL)
					}
				},
				complete: function(jqXHR, textStatus) {
					// remove loading feedback
					pInfScrNode.next().remove();
					
					// if our XHR did not add data to the page,
					if(pInfScrState != pInfScrStates.success) {
						// unhide 'more' browser
						pInfScrNode.show();
					}
					
					// unblock more requests (reset loading status)
					pInfScrState = pInfScrStates.idle;
				},
				dataType: "html"
			});	
		}
	}
}

$(document).ready(function () {
	// (4)
	// http://unscriptable.com/index.php/2009/03/20/debouncing-javascript-methods/
	// http://paulirish.com/2009/throttled-smartresize-jquery-event-handler/
	// http://benalman.com/projects/jquery-dotimeout-plugin/
	$(window).scroll(function(){
		$.doTimeout( 'scroll', pInfScrDelay, pInfScrExecute);
		// ensure that, when we're outside the target area, delay is set nice and low
		// this counteracts the longer delay set at window.onpopstate below
		if( $(document).height() - 200 > $(document).scrollTop() + $(window).height() )
			pInfScrDelay = 200;
	});
	
	// (6)
	window.onpopstate = function(event) {
		if(pInfScrIntraPageBackButton == 1) {	
			// we kept track of the number items returned last time...
			if(pInfScrCount.length > 0)
				var lastTime = pInfScrCount.pop();	

			// remove previous articles tacked on by scrolling
			// here, articles are defined as the first child of 
			// the body -- if this definition is not used, then
			// this script will also remove comments (which are
			// marked up as articles) from the end of the document
			// and fail to remove the required number of articles
			for(i=1;i<=lastTime;i++) {
				$("body > article").last().remove();
			}

			// browser will then end up at end of last page, reshow the navigation
			$('article#more').last().show();

			// popstate is here to manage infinite scroll navigation:
			// (1) user scrolled to target zone at bottom of the page
			// (2) more content was loaded
			// (3) user hit back button
			// when (3) occurs, the user is put back to the target zone in (1)
			// we need to delay the occurence of (2), giving the user time to think
			// about where they want to go and what they'd like to do
			pInfScrDelay = 3000;
		}
	};
});