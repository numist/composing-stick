<?

require_once(BLOGROOT."lib/post.class.inc");

class Link {
  public static function tag($arr) {
    if($arr == null) {
      trigger_error("no tags for link?", E_USER_WARNING);
      return Link::index();
    }

    if(!is_array($arr)) $arr = array($arr);
    return WEBROOT."tag/?".implode("&", $arr);
  }
  
  public static function post($post, $local=false) {
    assert('is_object($post)');

    if($local)
      return WEBROOT."post/".$post->location().".html";

    return $post->link();
  }
  
  public static function index() {
    return WEBROOT;
  }
  
  public static function about() {
    return WEBROOT."about.html";
  }
  
  public static function browse() {
    return WEBROOT."browse.html";
  }
  
  public static function feed($tags=null) {
    if($tags == null || (is_array($tags) && count($tags) == 0)) {
      return WEBROOT."feed.atom";
    }
    
    if(!is_array($tags)) $tags = array($tags);
    return WEBROOT."feed/?".implode("&", $tags);
  }
}

?>