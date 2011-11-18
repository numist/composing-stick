<?

define("BLOGROOT", dirname($argv[0])."/../");
define("WEBROOT", "http://numist.net/");
require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

assert($db);
assert($argc > 1);
$location = $argv[1];
assert('strpos($location, "\'") === false');

$post = new Post($location);

echo ($post->published() ? "updating" : "adding");
echo " ".$post->location()."\n";

?>