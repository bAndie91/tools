#!/usr/bin/env php
<?php

$err_rep = isset($argv[1]) ? $argv[1] : error_reporting();

foreach(get_defined_constants() as $const => $val)
{
	if(preg_match('/^E_/', $const))
	{
		$mask = ($err_rep & $val);
		if($mask == $val) $c = '';
		elseif($mask == 0) $c = '~';
		else $c = '#';

		echo "$c$const\n";
	}
}

