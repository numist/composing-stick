<?

assert('is_array($posts)');

if(count($posts) == 0) {
	trigger_error("no posts to syndicate", E_USER_WARNING);
	exit(0);
}

function str_contains($haystack, $needles) {
    if(!is_array($needles)) {
        $needles = array($needles);
    }
    foreach($needles as $needle) {
        if(strpos($haystack, $needle)) return true;
    }
    return false;
}

$lateststamp = 0;
foreach($posts as $post)
	if($lateststamp < $post->updatedstamp(true))
		$lateststamp = $post->updatedstamp(true);

?><<?="?"?>xml version="1.0" encoding="utf-8"<?="?"?>>
<feed xmlns="http://www.w3.org/2005/Atom">
  <title type="text">numist the fool</title>
  <? /* <subtitle type="html">
    A &lt;em&gt;lot&lt;/em&gt; of effort
    went into making this effortless
  </subtitle> */ ?>
  <updated><?= date("c", $lateststamp) ?></updated>
  <id>http://numist.net/</id>
  <link rel="alternate" type="text/html"
   hreflang="en" href="http://numist.net/"/>
  <link rel="self" type="application/atom+xml"
   href="http://numist.net/feed.atom"/>
  <rights>© <?= "2009 - ".date("Y") ?>, Scott Perry</rights>
  <generator uri="http://numist.net/about.html" version="0.1">
    Composing Stick
  </generator>

<? foreach($posts as $post) { ?>
  <entry>
    <title><?= $post->title() ?></title>
    <link rel="alternate" type="text/html"
     href="<?= Link::post($post) ?>"/>
    <id>tag:numist.net,<?= date("Y-m-d", $post->timestamp(true)) ?></id>
    <updated><?= date("c", $post->updatedstamp(true)) ?></updated>
    <published><?= date("c", $post->timestamp(true)) ?></published>
    <author>
      <name>Scott Perry</name>
      <uri>http://numist.net/</uri>
      <email>spam@numist.net</email>
    </author>
    <? /* <contributor>
      <name>Sam Ruby</name>
    </contributor>
    <contributor>
      <name>Joe Gregorio</name>
    </contributor> 
    
    
    ok, really I should only have the warning if the post contains script, video, ??? tags?  RESEARCH!
    */ ?>
    <content type="html">
      <? if(str_contains($post->htmlContent(), array("<script", "<audio", "<video", "<object", "<embed", "<cite", "textmate-source mac_classic"))) { ?>
        &lt;p style="color:#999">&lt;b>Note:&lt;/b>
        This blog post may contain video or JavaScript effects that are illegal in feeds and will be removed by most readers/aggregators. 
        Some styling may also be lost, but it should display perfectly if you &lt;a style="color:#999" href="<?= Link::post($post) ?>">open it in your browser&lt;/a>.&lt;/p>
      <? } ?>
      <?= str_replace(array("&", "<"), array("&amp;", "&lt;"), $post->htmlContent()) ?>
    </content>
  </entry>
<? } ?>
</feed>