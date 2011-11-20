<?
// XXX: I should write an error handler function that is fail-fast.
error_reporting(E_ALL | E_STRICT);
assert_options(ASSERT_ACTIVE,   true);
assert_options(ASSERT_BAIL,     true);

/* the optional nature of magic quotes make them dangerous to rely on, and
 * stupid to have available.  all magic quotes are deprecated in PHP, but
 * we should always make sure that our environment is as we expect, instead
 * of relying on assumptions that may not hold.
 */
if(function_exists("get_magic_quotes_runtime"))
  assert('!get_magic_quotes_runtime()');
if(function_exists("get_magic_quotes_gpc"))
  assert('!get_magic_quotes_gpc()');
if(function_exists("get_magic_quotes_sybase"))
  assert('!get_magic_quotes_sybase()');

/* dates are all relative or timezoneless, so this is a bit silly, but
 * PHP requires it.
 */
date_default_timezone_set('America/Los_Angeles');

/* composing stick relies on the availability and formatting of a few constants
 * and paths.  this file is included at the beginning of every page, and this
 * bank of asserts makes sure that the file has done the work required to put
 * us in a state where we can get work done.
 */
assert('defined("WEBROOT")');
assert('defined("BLOGROOT")');
assert('substr(BLOGROOT, -1) == DIRECTORY_SEPARATOR');
assert('substr(WEBROOT, -1) == "/"');
assert('file_exists(BLOGROOT."data/blog.db")');

require_once(BLOGROOT."lib/link.class.inc");
require_once(BLOGROOT."lib/post.class.inc");

// PHP really has become this weird amalgamation of Java, C, and Perl…
try {
  $db = new PDO("sqlite:".BLOGROOT."data/blog.db");
} catch (PDOException $e) {
  trigger_error($e->getMessage(), E_USER_ERROR);
}

/* sanitize_path takes a string representing some sort of path and removes
 * .. and . portions, cleaning up something like "../lib/bin/../foo/.." to
 * "../lib".
 */
function sanitize_path($path, $sep=DIRECTORY_SEPARATOR) {
	$path = explode($sep, $path);
	for($i = 1; $i < count($path); $i++) {
		if($path[$i] == ".." && $i > 0) {
			$path[$i] = false;
			$path[$i - 1] = false;
			$i -= 2;
			$path = ordered_array_cleanup($path);
		} else if($path[$i] == ".") {
			$path[$i] = false;
			$i -= 1;
			$path = ordered_array_cleanup($path);
		}
	}
	if(count($path) == 0) return ".";
	return implode($sep, $path);
}

/* for a list-type array (no string keys), this function returns an array
 * similar to the argument, but with all (bool)false elements removed.
 * note that other falsy elements are preserved—the comparison is strict.
 */
function ordered_array_cleanup($irr) {
  foreach($irr as $key => $val)
    assert('is_numeric($key) && $key == round($key)');
  
	$arr = array();
	for($i = 0; $i < count($irr); $i++) {
		if($irr[$i] !== false) {
			$arr[] = $irr[$i];
		}
	}
	return $arr;
}

// return the HTMLified post <div> with all the dressings.
function poast($post, $full=false) {
	$ret = "\n".'<article><div class="entry" id="'.str_replace(DIRECTORY_SEPARATOR, "-", $post->location()).'"><div class="entrycategory"><p>';
	foreach($post->tags() as $tag) {
		$ret .= ' <a href="'.Link::tag($tag).'"><img src="'.WEBROOT.'images/tags/'.$tag.'.png" border="0" width="16" height="16" /></a> ';
	}
	$ret .=	'</p></div><div class="entrybody"';
	if(function_exists("postwidth")) {
		$ret .= ' style="width:'.(postwidth() + 13).'px"';
	}
	$ret .= '>';
	if(!$full) {
		$ret .= $post->shortContent();
	} else {
		$ret .= $post->htmlContent();
	}
	$ret .=	'</div>';

	if(!$full && $post->hasMore()) {
		$ret .=	'<div class="more"><a href="'.Link::post($post, "local").'">more…</a></div>';
	}

	$ret .=	'<div class="entrydate"><p class="entrydate"><a href="'.Link::post($post, "local").'">posted on '.$post->timestamp().'</a><br /></p></div><br style="clear:both;" /></div></article>'."\n\n";
	
	return $ret;
}

// if running on the command line, print to standard error.
function stderr($str) {
  if(!defined('STDERR')) return;
  fwrite(STDERR, $str);
}

function array_keys_exist($keys, $arr, $logic="all") {
  foreach($keys as $key) {
    if(!array_key_exists($key, $arr) && $logic == "all")
      return false;
    if(array_key_exists($key, $arr) && $logic == "any")
      return true;
  }
  return $logic == "all" ? true : false;
}

function id($id) {
	return $id;
}

function array_top($arr) {
	return reset($arr);
}

function filename_append($fname, $str) {
	return substr($fname, 0, strrpos($fname, ".")).$str.substr($fname, strrpos($fname, "."));
}

?>