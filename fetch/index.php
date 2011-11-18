<?

define("BLOGROOT", "../");
define("WEBROOT", "../");

require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

$tags = array();
if(array_key_exists("tags", $_GET)) {
  $tags = json_decode($_GET["tags"]);

  // PHP 5.3.0+
  if(function_exists("json_last_error"))
    assert('JSON_ERROR_NONE == json_last_error()');
  else
    assert('$tags !== null');

  assert('$tags !== null');
  assert('is_array($tags)');
}

Post::checkTags($tags);

$num = 10;
if(array_key_exists("num", $_GET)) {
  $num = $_GET["num"];
  assert('is_numeric($num) && round($num) == $num');
}

$before = 0;
if(array_key_exists("before", $_GET)) {
  $before = $_GET["before"];
  if(!is_numeric($before)) $before = strtotime($before);
  assert('$before !== false');
}

$after = 0;
if(array_key_exists("after", $_GET)) {
  $after = $_GET["after"];
  if(!is_numeric($after)) $after = strtotime($after);
  assert('$after !== false');
}

$full = false;
if(array_key_exists("full", $_GET)) {
  $full = (bool)$_GET["full"];
}

$posts = Post::getPosts($tags, $num, $before, $after);

$jposts = array();
foreach($posts as $post) {
  $jposts[] = array(str_replace(DIRECTORY_SEPARATOR, "-", $post->location()), poast($post, $full));
}

echo json_encode($jposts);

?>