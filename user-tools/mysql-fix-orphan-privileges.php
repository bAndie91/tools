#!/usr/bin/env php
<?php

/*

=pod

=head1 NAME

mysql-fix-orphan-privileges.php - Suggest SQL commands to clean up unused records in system tables which hold permission data

=cut

*/


error_reporting(E_ALL & ~E_DEPRECATED);
ini_set('display_errors', true);

if(!isset($argv[3]))
{
    echo "Usage: {$argv[0]} host[:port] user password

This tool outputs sql commands to delete orphan records in privilege-related
tables, which belong to nonexiststent mysql users. This tool does not modify
any privileges, just suggests commands.

Connecting user must have SELECT privilege for `user`, `db`, `tables_priv`
and `columns_priv` tables in mysql database. You need DELETE privilege for
those last three tables in order to apply changes.
";
    exit(1);
}

if(!mysql_connect($argv[1], $argv[2], $argv[3])) die();
mysql_select_db("mysql");


function table($query)
{
        $return = array();
        $result = mysql_query($query);
        if(!$result) return NULL;
        while($row = mysql_fetch_assoc($result))
        {
                $return[] = $row;
        }
	mysql_free_result($result);
        return $return;
}

foreach(array("db", "tables_priv", "columns_priv") as $table)
{
        $grants[$table] = table("SELECT user, host FROM $table");
}

// print_r($grants);

foreach($grants as $table => $users)
{
    foreach($users as $user_data)
    {
	$user = $user_data["user"];
	$host = $user_data["host"];
	$user_safe = mysql_escape_string($user);
	$host_safe = mysql_escape_string($host);
	
	$result = mysql_query("SELECT 1 FROM user WHERE user='$user_safe' AND host='$host_safe' LIMIT 1");
	if($result and mysql_num_rows($result) == 0)
	{
	    echo "DELETE FROM $table WHERE user='$user_safe' AND host='$host_safe';\n";
	}
	mysql_free_result($result);
    }
}

echo "FLUSH PRIVILEGES;\n";
