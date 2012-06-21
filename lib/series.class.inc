<?

require_once(BLOGROOT."lib/post.class.inc");

class Series {
	private $data;

	public function __construct($location) {
      	assert('is_string($location)');
        assert('substr($location, -1) != DIRECTORY_SEPARATOR');
      	$this->data["location"] = $location;
      	$this->data["path"] = BLOGROOT."data/series/".$location;
        assert('is_dir($this->data["path"])');
        // In case the assert() doesn't kill execution, for whatever reason.
        if(!is_dir($this->data["path"])) {
	      	$this->data["location"] = null;
    	  	$this->data["path"] = null;
        }
    }

    public function id() {
    	return $this->data["location"];
    }

    public function title() {
      	if(!array_key_exists("title", $this->data)) {
        	if(file_exists($this->path()."/title.txt")) {
    	    	$title = trim(file_get_contents($this->path()."/title.txt"));
        		if(strlen($title) > 0) {
	            	$this->data["title"] = $title;
	            	return $this->data["title"];
	        	}
        	}
            $this->data["title"] = str_replace(array("-", "_", "+"), " ", basename($this->location()));
        }
        
        return $this->data["title"];
    }

    public function less() {
      	if(!array_key_exists("less", $this->data)) {
        	if(file_exists($this->path()."/less.html")) {
    	    	$less = trim(file_get_contents($this->path()."/less.html"));
        		if(strlen($less) > 0) {
	            	$this->data["less"] = $less;
	            	return $this->data["less"];
	        	}
        	}
        	trigger_error($this->path()."/less.html empty or not found", E_USER_WARNING);
            $this->data["less"] = "";
        }
        
        return $this->data["less"];
    }

    public function more() {
      	if(!array_key_exists("more", $this->data)) {
        	if(file_exists($this->path()."/more.html")) {
    	    	$more = trim(file_get_contents($this->path()."/more.html"));
        		if(strlen($more) > 0) {
	            	$this->data["more"] = $more;
	            	return $this->data["more"];
	        	}
        	}
        	trigger_error($this->path()."/more.html empty or not found", E_USER_WARNING);
            $this->data["more"] = "";
        }
        
        return $this->data["more"];
    }

    public function link() {
        return WEBROOT."series/".$this->id()."/";
    }

    private function path() {
    	// guaranteed by ctor
    	return $this->data["path"];
  	}

}

?>