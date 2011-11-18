String.prototype.trim = function () {
  return this.replace(/^\s*/, "").replace(/\s*$/, "");
}

String.prototype.basename = function() {
  return this.replace(/^.*\//, '');
}

Node.prototype.insertAfter = function(newNode, refNode) {
	if(refNode.nextSibling) {
		return this.insertBefore(newNode, refNode.nextSibling);
	} else {
		return this.appendChild(newNode);
	}
}

var scripts = document.getElementsByTagName('script');
var tracked_files = tracked_files || {};
// unfortunately, Javascript doesn't do __FILE__
var this_file = "include_once.js"

for(var i = 0, ii = scripts.length; i < ii; i++) {
  if(!scripts[i].src || scripts[i].iterated) continue;
  scripts[i].iterated = "yes";

  /* if there's a # in the filename and the preceding portion is this file's name,
   * then include the string after the # as the script to guard.
   */
  var files = scripts[i].src.split('#', 2);
  if(files.length == 2 && files[0].basename().trim() == this_file) {
    //console.log("called to include: "+files[1]);

    // only load each script once.
    if(tracked_files[files[1]]) continue;
    tracked_files[files[1]] = files[1];

    // first time!
    var newcontent = document.createElement('script'); 
    newcontent.src = files[1];
    newcontent.type = "text/javascript";
    newcontent.charset = "utf-8";
    newcontent.iterated = "yes";
    scripts[i].parentNode.insertAfter(newcontent, scripts[i]);
    //console.log("included file: "+files[1]);
  }
}