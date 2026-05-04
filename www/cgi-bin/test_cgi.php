#!/usr/bin/env php
<?php

header("Content-Type: text/html");

echo "<html><body>";
echo "<h1>CGI Test</h1>";

echo "<h2>Environment Variables</h2>";
echo "<pre>";
foreach ($_SERVER as $key => $value) {
    echo $key . " = " . $value . "\n";
}
echo "</pre>";

echo "<h2>GET Data</h2>";
echo "<pre>";
print_r($_GET);
echo "</pre>";

echo "<h2>POST Data</h2>";
echo "<pre>";
print_r($_POST);
echo "</pre>";

echo "<h2>Raw Input</h2>";
echo "<pre>";
echo file_get_contents("php://input");
echo "</pre>";

echo "</body></html>";
?>
