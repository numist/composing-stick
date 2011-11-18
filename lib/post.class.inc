<?

require_once(BLOGROOT."lib/simple_html_dom.inc");

class Post {
  private $data;

	public function __construct($location, $use_db=true) {
		global $db;
		$this->data = array();
		
		if(is_array($location)) {
		  Post::checkArgs($location);
		  assert('array_key_exists("location", $location)');
		  foreach($location as $key => $val)
		    $this->data[$key] = $val;
		}
		else {
		  assert('is_string($location)');
		  $this->data["location"] = $location;
	  }

	  $this->data["postpath"] = BLOGROOT."data/".$this->data["location"];
		assert('is_dir($this->data["postpath"])');

    /* if we have a db handle, it's faster to get data (if any exists) from
     * there than from the fs. prefetch it.
     */
    if($use_db && $db && !array_keys_exist(array("location", "timestamp", "tags", "content"), $this->data)) {

      /* if any of our datas have been set already, this means we already
       * wasted time on an imcomplete array argument. let the runner know and
       * continue.
       */
      if(is_array($location) && array_keys_exist(array("location", "timestamp", "tags", "content"), $this->data, "any")) {
        trigger_error("array argument to Post was incomplete!", E_USER_NOTICE);
      }

      // singleton post datas from db
		  $sql = "SELECT * FROM posts WHERE location == '".str_replace("'", "''", $this->data["location"])."'";
		  $res = $db->query($sql, PDO::FETCH_ASSOC);
		  assert('$res !== false && count($res) == 1');
		 
	    $arr = $res->fetch();
	    $res->closeCursor();
	    unset($res);

      if($arr === false) {
				return;
			}
      
      Post::checkArgs($arr);
      
      foreach($arr as $key => $val) {
        switch($key) {
          case "timestamp":
            $this->data["timestamp"] = strtotime($val);
            break;
          default:
		        $this->data[$key] = $val;
		        break;
        }
      }
      unset($arr);

	    // tags from db
		  if(is_numeric($this->data["postid"]) && !array_key_exists("tags", $this->data)) {
		    $res = $db->query("SELECT tag FROM post_tags WHERE postid = ".$this->data["postid"], PDO::FETCH_ASSOC);

    		$arr = array();
    		while(false !== ($row = $res->fetch()))
    			$arr[] = $row['tag'];
    		unset($row);
    		unset($res);

    		Post::checkTags($arr);

    		$this->data["tags"] = $arr;

    		unset($arr);
		  }
	  }

		return;
	}

  /***************************************************************************/
  /* static functions                                                        */
  /***************************************************************************/

  private static function checkArgs($arr) {
    assert('is_array($arr)');

    foreach($arr as $key => $val) {
      switch($key) {
        case "timestamp":
          assert('strtotime($val) > 0');
          break;
        case "tags":
          Post::checkTags($val);
          break;
        case "postid":
          assert('is_numeric($val) && $val == round($val)');
        case "content":
        case "location":
          break;
        default:
	        trigger_error("$key not an allowed data member of Post", E_USER_WARNING);
      }
    }
  }

  public static function checkTags($arr) {
    assert('is_array($arr)');

    foreach($arr as $tag) {
      // this can only happen when someone is trying to hack us.  tags don't contain path characters.
		  if(strpos($tag, DIRECTORY_SEPARATOR) !== false) trigger_error("bad dog, no biscuit", E_USER_ERROR);

      assert('file_exists(BLOGROOT."images/tags/$tag.png")');
    }
  }

	public static function getPosts($tag=null, $num=10, $before=0, $after=0) {
		global $db;
		// argument checking
		assert('is_numeric($num) && round($num) == $num');
		assert('is_numeric($before)');
		assert('is_numeric($after)');
		assert('$tag == null || is_array($tag)');

		if($before == 0) $before = time();
		$before = date("Y-m-d H:i:s", $before);
		$after = date("Y-m-d H:i:s", $after);
		
	  $where = "";
		// if we are getting specific tags, the query gets a lot more complicated,
		if($tag != null) {
		  $itags = array();
		  $tags = array();
		  
		  // separate the ones we want vs the ones we don't
		  for($i = 0; $i < count($tag); $i++) {
		    if(strpos($tag[$i], "-") !== false)
		      $itags[] = substr($tag[$i], 1);
		    else
		      $tags[] = $tag[$i];
	    }
	    unset($tag);
		  
		  // a where clause for the tags we want
		  if(count($tags)) {
		    $where .= "AND postid IN (SELECT postid FROM post_tags WHERE ";
      
		    for($i = 0; $i < count($tags); $i++) {
		      assert('file_exists(BLOGROOT."images/tags/".$tags[$i].".png")');
		      if(strpos($tags[$i], DIRECTORY_SEPARATOR) !== false) trigger_error("bad dog, no biscuit", E_USER_ERROR);

		      $where .= "tag == '".$tags[$i]."'";
		      if($i < count($tags) - 1)
		        $where .= " OR ";
		      else
		        $where .= ") ";
		    }
		  }
		  
		  // another where clause for the ones we don't
		  if(count($itags)) {
		    $where .= "AND postid NOT IN (SELECT postid FROM post_tags WHERE ";
		  
		    for($i = 0; $i < count($itags); $i++) {
		      assert('file_exists(BLOGROOT."images/tags/".$itags[$i].".png")');
		      if(strpos($itags[$i], DIRECTORY_SEPARATOR) !== false) trigger_error("bad dog, no biscuit", E_USER_ERROR);

		      $where .= "tag == '".$itags[$i]."'";
		      if($i < count($itags) - 1)
		        $where .= " OR ";
		      else
		        $where .= ") ";
		    }
		  }
		}

    // XXX: if we need efficiency in building many post objects, select * and modify the ctor to take arrays.
		$sql = "SELECT location FROM posts WHERE timestamp < '$before' AND timestamp > '$after' $where ORDER BY timestamp DESC LIMIT $num";
		$res = $db->query($sql, PDO::FETCH_ASSOC);
		$arr = array();

		while(false !== ($row = $res->fetch())) {
			$arr[] = new Post($row['location']);
		}

		return $arr;
	}

	public static function formatstamp($timestamp) {
		if(!is_numeric($timestamp)) {
			$timestamp = strtotime($timestamp);
		}
		assert('$timestamp > 0');
		
		$tstr = strtolower(date("l F jS, Y", $timestamp));
		$tstr = preg_replace("/(\d+)(st|nd|rd|th)/", '${1}<sup>${2}</sup>', $tstr);
		$tstr .= " at ";
		$tstr .= date("G:i", $timestamp);
		
		return $tstr;
	}

  /***************************************************************************/
  /* accessor functions for object data                                      */
  /***************************************************************************/

	public function postid() {
		if(!array_key_exists("postid", $this->data)) {
			return false;
		}
    assert(is_numeric($this->data["postid"]));
		return $this->data["postid"];
	}

	public function tags() {
	  if(!array_key_exists("tags", $this->data)) {
	    assert('file_exists($this->postpath()."/tags.txt")');
	    
  		$tags = file($this->postpath()."/tags.txt");
  		for($i = 0; $i < count($tags); $i++) {
  			$tags[$i] = trim($tags[$i]);
				if(strlen($tags[$i]) == 0) continue;
      
        // XXX: empty string?  do we ever run into that?
  			assert('file_exists(BLOGROOT."images/tags/".$tags[$i].".png")');
  			if(strpos($tags[$i], DIRECTORY_SEPARATOR) !== false) trigger_error("bad dog, no biscuit", E_USER_ERROR);
  		}
  		$this->data["tags"] = $tags;
	  }

		return $this->data["tags"];
	}

	public function location() {
	  // guaranteed by ctor
		return $this->data["location"];
	}

  private function postpath() {
    // guaranteed by ctor
    return $this->data["postpath"];
  }

	public function link() {
		if(file_exists(BLOGROOT."data/".$this->location()."/link.txt"))
			return file_get_contents(BLOGROOT."data/".$this->location()."/link.txt");
		return WEBROOT."post/".$this->location().".html";
	}

	public function timestamp($asnum=false) {
	  if(!array_key_exists("timestamp", $this->data)) {
	    assert('file_exists($this->postpath()."/timestamp.txt")');
	    $tstamp = strtotime(file_get_contents($this->postpath()."/timestamp.txt"));
	    assert('$tstamp > 0');

  		$this->data["timestamp"] = $tstamp;
	  }

		if($asnum) return $this->data["timestamp"];

		return Post::formatstamp($this->data["timestamp"]);
	}
	
	public function title() {
	  if(!array_key_exists("title", $this->data)) {
	    if(file_exists($this->postpath()."/title.txt")) {
	      $title = trim(file_get_contents($this->postpath()."/title.txt"));
	      if(strlen($title) > 0)
	        $this->data["title"] = $title;
	    }
		  else {
		  	$this->data["title"] = str_replace(array("-", "_", "+"), " ", basename($this->location()));
		  }
		}
		
		return $this->data["title"];
	}
	
	private function content() {
	  if(!array_key_exists("content", $this->data)) {
	    assert('file_exists($this->postpath()."/post.html")');
  		$this->data["content"] = file_get_contents($this->postpath()."/post.html");
	  }
	  
	  return $this->data["content"];
	}

	/***************************************************************************/
	/* formatting/logic functions                                              */
	/***************************************************************************/

	public function updatedstamp($asnum=false) {
		// edits - functionalize this at some point
		$html = str_get_html($this->content());
		$edits = $html->find("edit");
		// sigh. no library is perfect. no matches should = empty array.
		if($edits === null) return $this->timestamp($asnum);

		$latest = $this->timestamp(true);

		foreach($edits as $edit) {
			if(strtotime($edit->time) > $latest) {
				$latest = strtotime($edit->time);
			}
		}

		if($asnum) return $latest;

		return Post::formatstamp($latest);
	}

	public function htmlContent($fullpost=true) {
	  $content = $this->content();
	  
    // generating htmlContent can be slow, so we cache!
    $md5 = md5($content.WEBROOT.BLOGROOT);
    $loc = str_replace(DIRECTORY_SEPARATOR, "-", $this->location());
    $tmpnam = BLOGROOT."tmp/$loc-$md5-".($fullpost ? "f" : "s");
    if(is_dir(BLOGROOT."tmp") && file_exists($tmpnam)) {
      if(is_writable($tmpnam))
        touch($tmpnam);
      return file_get_contents($tmpnam);
    }
	  
    $start = microtime(true);

		/* cut up the post as necessary.  short posts display everything before
		 * <!-- more -->, long posts display everything except content between
		 * <!-- less --> and <!-- more -->
		 */
		if(!$fullpost) {
			if(strpos($content, "<!-- more -->")) {
				$content = substr($content, 0, strpos($content, "<!-- more -->"));
			}
		} else if(strpos($content, "<!-- more -->") && strpos($content, "<!-- less -->") !== false) {
			$content = substr($content, 0, strpos($content, "<!-- less -->")).substr($content, strpos($content, "<!-- more -->"));
		}

		// 1st, 2nd, 3rd, 4th, …
		$content = preg_replace("/(\d+)(st|nd|rd|th)/", '${1}<sup>${2}</sup>', $content);

		// lists and captions
		$find = array(	"<li>",
						"</li>",
						"<caption>",
						"</caption>"
		);
		$repl = array(	"<li><span class=\"list\">",
						"</span></li>",
						"<div class=\"caption\">",
						"</div>"
		);
		$content = str_replace($find, $repl, $content);

    // image width and heights for local images
    $html = str_get_html($content);
    $imgs = $html->find("img");
    // sigh. no library is perfect. no matches should = empty array.
    if($imgs === null) $imgs = array();
    
    foreach($imgs as $img) {
      if(isset($img->src)) {
        if(strpos($img->src, "local://") === 0) {
          list($width, $height, $type, $attr) = getimagesize(BLOGROOT."data/".$this->location().substr($img->src, 7));
          $img->width = $width;
          $img->height = $height;
        } else if(strpos($img->src, "root://") === 0) {
          list($width, $height, $type, $attr) = getimagesize(BLOGROOT.substr($img->src, 7));
          $img->width = $width;
          $img->height = $height;
        }
      }
    }
    $content = $html->save();

		// links to local files made sensible
		$content = str_replace("local://", WEBROOT."data/".$this->location()."/", $content);
		$content = str_replace("root://", WEBROOT, $content);

		// edits!
		$content = $this->doEdits($content);
		
		// footnotes!
		$content = $this->doFootnotes($content);
		
    stderr($this->location().": ".number_format(microtime(true) - $start, 3)."s\n");
		
    if(is_writable(BLOGROOT."tmp") && microtime(true) - $start > 0.05) {
      file_put_contents($tmpnam, $content);
    }

		return $content;
	}

	private function doFootnotes($content) {
		$location = $this->location();

		// parse the post into a DOM tree
		$html = str_get_html($content);

		// find all ref tags
		$refs = $html->find("ref");
		// sigh. no library is perfect. no matches should = empty array.
		if($refs === null) $refs = array();

		$footnotes = array();
		$index = 0;

		foreach($refs as $ref) {
		    // labelled reference
		    if($ref->label != null) {
		        if(!in_array("label:".$ref->label, $footnotes)) {
		            // if the label doesn't exist on record yet, create it
		            $footnotes[$index++] = "label:".$ref->label;
		            $lndes = $index - 1;
		        } else {
		            // otherwise, find its index
		            $lndes = array_search("label:".$ref->label, $footnotes);
		        }
		        if($ref->innertext) {
		            /* if a labelled ref has innertext, it is not itself a
		             * footnote marker and should not be displayed. */
		            // save the footnote text
		            $footnotes[$lndes] = $ref->innertext;
		            // destroy the tag
		            $ref->outertext = '';
		            continue;
		        }
		    } else {
		        /* unlabelled footnotes must contain their foottext within
		         * the ref tags.
		         */
		        $footnotes[$index++] = $ref->innertext;
		    }
		    $ref->outertext = "<sup><a href=\"#".$location."/fn$index\">$index</a></sup>";
		}
		// oh yeah, nested <ref> tags: don't :(

		// write the (possibly) modified DOM back to text.
		$content = $html->save();

		// now that we have the footnotes, emit them at the end of the post.
		if(count($footnotes) > 0) {
		    $content .= "\n\n<div class=\"footnotes\"><hr />\n";
		    foreach($footnotes as $footnum => $foottext) {
		      // ensure all footnotes are matched
	        assert(strpos($foottext, "label:") !== 0);

	        $content .= "<a name=\"".$location."/fn".($footnum + 1)."\"></a><sup>"
	                 .($footnum + 1)."</sup> ".$foottext."<br />\n";
		    }
		    $content .= "</div>\n";
		}

		// get around a php memory leak involving circular references.
		$html->clear();
		unset($html);

		return $content;
	}

	private function doEdits($content) {
		// edits - functionalize this at some point
		$html = str_get_html($content);
		$edits = $html->find("edit");
		// sigh. no library is perfect. no matches should = empty array.
		if($edits === null) $edits = array();

		foreach($edits as $edit) {
			$time = $edit->time;
			
			$edit->outertext = '<div class="edit">
				<div class="editdate"><p class="editdate">edited on '.Post::formatstamp($time).' at 11:58 :</p></div>
				<div class="editreason">'.$edit->innertext.'</div>
			</div>';
		}
		
		// get around a php memory leak involving circular references.
		$ret = $html->save();
		$html->clear();
		unset($html);
		return $ret;
	}

	public function shortContent() {
		return $this->htmlContent(false);
	}

	public function hasMore() {
		if(strpos($this->content(), "<!-- more -->")) {
			return true;
		} else {
			return false;
		}
	}
}

?>
