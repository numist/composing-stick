<?

require_once(BLOGROOT."lib/simple_html_dom.inc");

class Post {
  private $data;

    public function __construct($location) {
        global $db;

        assert('is_string($location)');
        assert('substr($location, -1) != DIRECTORY_SEPARATOR');
        $this->data["location"] = $location;
        $this->data["postpath"] = BLOGROOT."data/".$location;
        assert('is_dir($this->data["postpath"])');

        if($db && !array_keys_exist(array("location", "timestamp", "series", "content"), $this->data)) {
            // singleton post datas from db
            $sql = "SELECT * FROM post_index WHERE location == '".str_replace("'", "''", $this->data["location"])."'";
            $res = $db->query($sql, PDO::FETCH_ASSOC);
            assert('$res !== false && count($res) == 1');
            $arr = $res->fetch();
            $res->closeCursor();
            unset($res);

            if($arr === false) {
                $this->data["in_db"] = false;
            } else {
                $this->data["in_db"] = true;
            }
        }
        return;
    }

  /***************************************************************************/
  /* static functions                                                        */
  /***************************************************************************/

    public static function getPages($start, $pages) {
        return POST::getPosts(null, (($start - 1) * 10).",".($pages * 10));
    }

    public static function getPosts($series=null, $num=10, $before=0, $after=0) {
        global $db;
        // argument checking
        assert('preg_match("/[0-9]+,?[0-9]*/", $num) == 1');
        assert('is_numeric($before)');
        assert('is_numeric($after)');
        assert('$series == null || id(new Series($series))->id()');

        if($before == 0) $before = time();
        
        // if we are getting a specific series, the query gets a lot more complicated,
        if($series != null) {
            $id = id(new Series($series))->id();

            $where = "AND (series LIKE '%:$id:%' OR series LIKE '%:$id' OR series LIKE '$id:%' OR series = '$id')";

            $order = "ORDER BY time ASC";
        } else {
            $where = "";

            $order = "ORDER BY time DESC";
        }

        $sql = "SELECT location FROM post_index WHERE time < '$before' AND time > '$after' $where $order LIMIT $num";
        $arr = array();
        $countsql = "SELECT COUNT(*) ".substr($sql, strpos($sql, "FROM"));
        $countsql = substr($countsql, 0, strrpos($countsql, "ORDER"));
        $res = $db->query($countsql, PDO::FETCH_ASSOC);
        if ($res && $res->fetchColumn() > 0) {
            $res = $db->query($sql, PDO::FETCH_ASSOC);

            while(false !== ($row = $res->fetch())) {
                $arr[] = new Post($row['location']);
            }
        }

        return $arr;
    }

    public static function formatstamp($timestamp) {
        assert('is_string($timestamp)');
        assert('strtotime($timestamp) > 0');
        
        $date = new DateTime($timestamp);

        $tstr = strtolower($date->format("l F jS, Y"));
        $tstr = preg_replace("/(\d+)(st|nd|rd|th)/", '${1}<sup>${2}</sup>', $tstr);
        $tstr .= " at ";
        $tstr .= $date->format("G:i (T)");
        
        return $tstr;
    }

  /***************************************************************************/
  /* accessor functions for object data                                      */
  /***************************************************************************/

    public function series($query=false) {
        if(!array_key_exists("series", $this->data)) {
            if(!file_exists($this->postpath()."/series.txt")) return array();
            
            $series = file($this->postpath()."/series.txt");
            $ret = array();
            for($i = 0; $i < count($series); $i++) {
                if(strlen(trim($series[$i])) == 0) continue;

                if($query) {
                    $ret[] = trim($series[$i]);
                } else {
                    $ret[] = new Series(trim($series[$i]));
                }
            }
            if($query) {
                $ret = implode(":", $ret);
            }
            $this->data["series"] = $ret;
        }

        return $this->data["series"];
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
        $fstamp = file_get_contents($this->postpath()."/timestamp.txt");
        $tstamp = strtotime($fstamp);
        assert('$tstamp > 0');

        $this->data["timestamp"] = $tstamp;
        $this->data["filestamp"] = $fstamp;
      }

      if($asnum) return $this->data["timestamp"];

      return Post::formatstamp($this->data["filestamp"]);
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

    public function published() {
        return $this->data["in_db"];
    }
    
    public function width() {
        if(!array_key_exists("width", $this->data)) {
            $thewidth = 580;
            $html = str_get_html($this->htmlContent());
            $imgs = $html->find("img");
            // sigh. no library is perfect. no matches should = empty array.
            if($imgs === null) $imgs = array();

            foreach($imgs as $img) {
                if(isset($img->src)) {
                    if(strpos($img->src, WEBROOT) === 0) {
                        $img->src = BLOGROOT.substr($img->src, strlen(WEBROOT));
                    } else {
                        trigger_error($img->src." doesn't start with ".WEBROOT."? in ".$_SERVER["SCRIPT_FILENAME"], E_USER_WARNING);
                    }
                    list($width, $height, $type, $attr) = getimagesize($img->src);
                    if($width > $thewidth) $thewidth = $width;
                }
            }
            
            $this->data["width"] = $thewidth;
        }
        
        return $this->data["width"];
    }
    
    private function content() {
      if(!array_key_exists("content", $this->data)) {
        assert('file_exists($this->postpath()."/post.html")');
        $this->data["content"] = file_get_contents($this->postpath()."/post.html");
      }
      
      return $this->data["content"];
    }

    /***************************************************************************/
    /* action functions                                                        */
    /***************************************************************************/
    
    public function publish() {
        global $db;
        $timestamp = $this->timestamp(true);
        $location = $this->location();
        $series = $this->series(true);

        $db->query("DELETE FROM post_index WHERE location = '$location'");
        $db->query("INSERT INTO post_index(location, time, series) VALUES('$location', $timestamp, '$series')");
    }
    
    public function parse() {
        // get images that are non-local
    $html = str_get_html($this->content());
        $path = sanitize_path($this->postpath()).DIRECTORY_SEPARATOR;
        
    $imgs = $html->find("img");
    // sigh. no library is perfect. no matches should = empty array.
    if($imgs === null) $imgs = array();

    foreach($imgs as $img) {
      if(isset($img->src)) {
        if(strpos($img->src, "local://") !== 0 &&
                   strpos(urldecode($img->src), "root://") !== 0) {
                    
                    $fname = basename($img->src);
                    $i = 1;
                    while (file_exists($path.$fname)) {
                        $fname = filename_append(basename($img->src), "-".++$i);
                    }
                    stderr("caching ".$img->src."\n");
                    $imagedata = file_get_contents($img->src) or trigger_error("bad link: ".$img->src."?", E_USER_ERROR);
                    file_put_contents($path.$fname, $imagedata);
                    unset($imagedata);
                    $img->src = "local://$fname";
        }
      }
    }
        // save to disk
    $content = $this->data["content"] = $html->save();
        file_put_contents($path."post.html",
                          $this->data["content"]);
        
        // create other-sized images where required
        $short = strpos($content, "<!-- more -->") ?
                 substr($content, 0, strpos($content, "<!-- more -->")) :
                 $content;
        $long = strpos($content, "<!-- more -->") && strpos($content, "<!-- less -->") !== false ?
                substr($content, 0, strpos($content, "<!-- less -->")).substr($content, strpos($content, "<!-- more -->")) :
                $content;
        $sizes = array(
            array($short, 570, "-small"),
            array($long, 1024, "-medium")
        );
        
        foreach($sizes as $size) {
            $html = str_get_html($size[0]);

        $imgs = $html->find("img");
        // sigh. no library is perfect. no matches should = empty array.
        if($imgs === null) $imgs = array();

        foreach($imgs as $img) {
          if(isset($img->src) &&  strpos($img->src, "local://") === 0) {
                    $fname = substr($img->src, strlen("local://"));
                    $aname = filename_append($path.$fname, $size[2]);
                    list($width, $height, $type, $attr) = getimagesize($path.urldecode($fname));
                    if($width > $size[1] && !file_exists($aname)) {
                        exec('which convert', $out, $ret);
                        assert('$ret == 0');
                        $newwidth = $size[1];
                        $newheight = (int)($height * $newwidth / $width);
                        // RESIZE!
                        stderr("resizing $path$fname\n");
                        exec("convert \"".$path.$fname."\" -resize ".$newwidth."x".$newheight." \"".$aname."\"", $out, $ret);
                        assert('$ret == 0');
                    }
                }
            }
        }
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
        if (php_sapi_name() === "cli") {
            // if we are running cli, run the preprocessor
            $this->parse();
            // and update the related local variables
        }
        $content = $this->content();

        // generating htmlContent can be slow, so we cache!
        $md5 = md5($content.BLOGROOT.WEBROOT);
        $loc = str_replace(DIRECTORY_SEPARATOR, "-", $this->location());
        $tmpnam = BLOGROOT."tmp/$loc-$md5-".($fullpost ? "f" : "s");
        if(is_dir(BLOGROOT."tmp") && file_exists($tmpnam) && !php_sapi_name() === "cli") {
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
        $find = array(  "<li>",
                        "</li>",
                        "<caption>",
                        "</caption>"
        );
        $repl = array(  "<li><span class=\"list\">",
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
                    $postdir = BLOGROOT."data/".$this->location().DIRECTORY_SEPARATOR;
                    $fname = $ogname = substr(urldecode($img->src), strlen("local://"));
                    if(!$fullpost && file_exists($postdir.filename_append(urldecode($fname), "-small"))) {
                        $fname = filename_append($fname, "-small");
                    } else if(file_exists($postdir.filename_append(urldecode($fname), "-medium"))) {
                        $fname = filename_append($fname, "-medium");
                    }
                    $img->src = "local://".$fname;
    
                    list($width, $height, $type, $attr) = getimagesize($postdir.$fname);
                    $img->width = $width;
                    $img->height = $height;
                    if($img->parent()->tag != "a" && $ogname != $fname) {
                        $img->outertext = '<a href="local://'.$ogname.'">'.$img->outertext.'</a>';
                    }
                } else if(strpos(urldecode($img->src), "root://") === 0) {
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

        if(is_writable(BLOGROOT."tmp") && (microtime(true) - $start > 0.05 ||
                                           php_sapi_name() == "cli" ||
                                           $_SERVER["REMOTE_ADDR"] == "::1"))
        {
            file_put_contents($tmpnam, $content);
            if(php_sapi_name() == "cli") {
                stderr($this->location().": ".number_format(microtime(true) - $start, 3)."s\n");
            } else {
                trigger_error($this->location().": ".number_format(microtime(true) - $start, 3)."s", E_USER_WARNING);
            }
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
                if (strpos($foottext, "label:") === 0) {
                    trigger_error("missing label $foottext", E_USER_ERROR);
                }

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
                <div class="editdate"><p class="editdate">edited on '.Post::formatstamp($time).':</p></div>
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