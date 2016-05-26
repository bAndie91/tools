#!/usr/bin/env php
<?php

$session_files = array_slice($argv, 1, $argc - 1);
if(count($session_files) < 1 or $session_files[0] == '--help')
{
	die("Usage: $argv[0] files\n");
}

exec("mktemp -d", $output, $code);
if($code != 0) die($code);
$temp_dir = $output[count($output)-1];
echo "TMPDIR=$temp_dir" . PHP_EOL;
session_save_path($temp_dir);

foreach($session_files as $file)
{
	@session_start(); 
	session_decode(file_get_contents($file));
	print_r($_SESSION);
	session_destroy();
}

/*
exec("rm -r ".escapeshellarg($temp_dir), $output, $code);
if($code != 0) die($code);
*/
