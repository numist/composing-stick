<?

$preview = $argc > 2;

define("BLOGROOT", dirname($argv[0])."/../");
define("WEBROOT", "../../".($preview ? "../" : ""));

require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/series.class.inc");

assert('$argc == 2 || $argc == 3');
assert('is_dir(BLOGROOT."data/".$argv[1])');

$post = new Post($argv[1]);

function title() {
	global $post;
	return $post->title();
}

function nav() {
	return "<div class=\"nav\"><a href=\"".Link::index()."\">home</a></div>
			<div class=\"nav\" style=\"margin-left: 65px\"><a href=\"".Link::about()."\">about</a></div>";
}

function postwidth() {
	global $post;
	return $post->width();
}

require(BLOGROOT."templates/header.inc");

echo poast($post, true);

$series = $post->series();
shuffle($series);

if(count($series)) {?>
<div class="entry">
	<div class="entrybody">
		<p>This post is part of the series
			<? for($i = 0; $i < count($series); $i++) {
				// Three or more items, Oxford commas!
				if($i && count($series) > 2) {
					echo ",";
				}
				if($i) {
					echo " ";
				}
				// Last item, and it together
				if($i && $i == count($series) - 1) {
					echo "and ";
				}
				?><a href="<?= $series[$i]->link() ?>"><?= $series[$i]->title() ?></a><?
			}
		?>.</p>
	</div>
	<br style="clear:both;">
	<br style="clear:both;">
</div>
<?}

require(BLOGROOT."templates/footer.inc");


?>
