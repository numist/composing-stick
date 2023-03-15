<? assert('!isset($tags) || is_array($tags)'); ?><!DOCTYPE html>
<html lang="en">
<? global $preview; ?>
<head>
	<meta charset="utf-8">
	<title><? if(function_exists("title")) { echo title(); } else { echo "the emperor's new title"; } ?></title>
  <link rel="stylesheet" href="<?= WEBROOT ?>styles/main.css" type="text/css" media="screen" title="no title" charset="utf-8" />
  <link rel="stylesheet" href="<?= WEBROOT ?>styles/code.css" type="text/css" media="screen" title="no title" charset="utf-8" />
	<script type="text/javascript" src="<?= WEBROOT ?>js/jquery-1.7.1.min.js"></script>
	<script type="text/javascript" src="<?= WEBROOT ?>js/jsLoad-1.0.0.min.js"></script>
	<link rel="icon" type="image/png" href="<?= WEBROOT ?>favicon.ico">
	<? if(function_exists("head")) { echo head(); } ?>
</head>
<body>
	<div id="container"<? if(function_exists("postwidth")) echo ' style="width:'.(postwidth() + 43).'px;"'; ?>>
		<? if(function_exists("nav")) { echo nav(); } ?>
  		<div id="entries">
