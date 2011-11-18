<? assert('!isset($tags) || is_array($tags)'); ?><!DOCTYPE html>
<html>
<? global $preview; ?>
<head>
	<? if(function_exists("head")) { echo head(); } ?>
	<meta charset="utf-8">
	<title><? if(function_exists("title")) { echo title(); } else { echo "the emperor's new title"; } ?></title>
  	<link rel="stylesheet" href="<?= WEBROOT ?>styles/main.css" type="text/css" media="screen" title="no title" charset="utf-8" />
  	<link rel="stylesheet" href="<?= WEBROOT ?>styles/code.css" type="text/css" media="screen" title="no title" charset="utf-8" />
  	<meta id="iphone-viewport" content="width=620; maximum-scale=0.6667" name="viewport"/>

    <script src="<?= WEBROOT ?>js/helpers.js" type="text/javascript" charset="utf-8"></script>
  	<script src="<?= WEBROOT ?>js/raphael-min.js" type="text/javascript" charset="utf-8"></script>
  	<script src="<?= WEBROOT ?>js/jquery-1.4.2.min.js" type="text/javascript" charset="utf-8"></script>
  	<script src="<?= WEBROOT ?>js/json2.js" type="text/javascript" charset="utf-8"></script>
	<link rel="icon" type="image/png" href="<?= WEBROOT ?>favicon.ico">
</head>
<body>
  <div id="tags" style="display: none;" data-tags='<?= json_encode(isset($tags) ? $tags : array()) ?>' ></div>
	<div id="container">
		<? if(function_exists("nav")) { echo nav(); } ?>
  		<div id="entries">