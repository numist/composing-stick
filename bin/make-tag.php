<?

define("BLOGROOT", dirname($argv[0])."/../");
define("WEBROOT", "../");

require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

$tags = array();
for($i = 1; $i < $argc; $i++) {
  $tags[] = $argv[$i];
}

foreach($tags as $tag) {
  if(strpos($tag, "-") === 0) $tag = substr($tag, 1);
  assert('file_exists(BLOGROOT."images/tags/".$tag.".png")');
  if(strpos($tag, DIRECTORY_SEPARATOR) !== false) trigger_error("bad dog, no biscuit", E_USER_ERROR);
}

require(BLOGROOT."lib/tag.php");

?>