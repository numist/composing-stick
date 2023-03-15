<?

require_once(BLOGROOT."lib/post.class.inc");

class Link {
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
  
  public static function feed() {
    return WEBROOT."atom/";
  }
}

?>