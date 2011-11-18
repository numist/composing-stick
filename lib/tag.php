<?

assert('isset($tags) && is_array($tags)');

function title() {
  global $tags;
  $str = "";
  for($i = 0; $i < count($tags); $i++) {
    if(strpos($tags[$i], "-") === 0)
      $str .= "not ".substr($tags[$i], 1);
    else
      $str .= $tags[$i];

    if($i < count($tags) - 1)
      $str .= ", ";
  }
	return 'pigeonhole the fool: '.$str;
}

function nav() {
	return "<div class=\"nav\"><a href=\"".Link::index()."\">home</a></div>
			<div class=\"nav\" style=\"margin-left: 65px\"><a href=\"".Link::about()."\">about</a></div>
			<div class=\"nav\" style=\"margin-left: 130px\"><a href=\"".Link::browse()."\">browse</a></div>";
}

require(BLOGROOT."templates/header.inc");

$posts = Post::getPosts($tags);

foreach($posts as $post) {
	echo poast($post);
}

require(BLOGROOT."templates/footer.inc");

?>