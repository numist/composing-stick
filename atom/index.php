<?

define("BLOGROOT", "../");
define("WEBROOT", "../");

require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

$posts = Post::getPosts();

require(BLOGROOT."lib/atom.php");

?>