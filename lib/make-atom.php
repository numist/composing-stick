<?

define("BLOGROOT", dirname($argv[0])."/../");
define("WEBROOT", "http://numist.net/");
require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

assert('$argc == 1');

$posts = Post::getPosts();

require(BLOGROOT."lib/atom.php");

?>