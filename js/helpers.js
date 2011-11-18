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

String.prototype.asElementArray = function() {
  var foo = document.createElement('div');
  foo.innerHTML = this;
  return foo.childNodes;
}