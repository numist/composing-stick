<?

define("BLOGROOT", "../");
define("WEBROOT", "../");

require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

$args = explode("&", $_SERVER['QUERY_STRING']);

if(count($args) == 0)
  $posts = Post::getPosts();
else {
  Post::checkTags($args);
  $posts = Post::getPosts($args);
}

require(BLOGROOT."lib/atom.php");

?>