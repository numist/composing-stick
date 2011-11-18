<?

define("BLOGROOT", dirname($argv[0])."/../");
define("WEBROOT", "./");
require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

assert('$argc == 1');

function nav() {
	return '<div class="nav"><a href="'.Link::index().'">home</a></div>
			<div class="nav" style="margin-left: 65px"><a href="'.Link::about().'">about</a></div>';
}

function title() {
	return "browse the fool";
}


require(BLOGROOT."templates/header.inc");
?>
<div class="entry">
<h3>tags:</h3>
<p>darker tags have more posts.</p>
<p><?

$sql = "SELECT COUNT(*) AS count, tag FROM tag_index GROUP BY tag";
$res = $db->query($sql, PDO::FETCH_ASSOC);
$arr = array();
$highest = 0;
while(false !== ($row = $res->fetch())) {
	$arr[$row['tag']] = $row['count'];
	if($row['count'] > $highest) $highest = $row['count'];
}

// minimum colour: 50% gray
$min = 200;
// maximum colour: black
$max = 0;

foreach($arr as $tag => $count) {
	$color = dechex($max + ($highest - $count)*($min - $max)/$highest);
	if(strlen($color) == 1) $color = "0".$color;
	$color .= $color.$color; // grey!
	if($count == 1) {
		$title = "$count post";
	} else {
		$title = "$count posts";
	}

	?>
	<a href="<?= Link::tag($tag) ?>" style="text-decoration: none;" title="<?= $title ?>"><span style="color: #<?= $color ?>; white-space: nowrap;"><img src="<?= WEBROOT ?>images/tags/<?= $tag ?>.png" width="16" height="16" style="opacity: <?= number_format($count / $highest, 2) ?>; filter:alpha(opacity=<?= round(100 * ($count / $highest)) ?>);" align="absbottom">&nbsp;<?= $tag ?></span></a>&nbsp;&nbsp;
	<?
}

?></p>

	<p></p>
</div>

<div class="entry">
	<p>normally folks put archives by year or by month here, but I've never liked the interface.  <!-- if you have an inkling of what you're looking for, try searching for it instead (once there are enough posts to bother searching, there will be an easy-to-find search box).--></p>
	<!--<p>otherwise, lazyloading should take care of your historical needs.</p>-->
	<p></p>
</div>

<? require(BLOGROOT."templates/footer.inc"); ?>
