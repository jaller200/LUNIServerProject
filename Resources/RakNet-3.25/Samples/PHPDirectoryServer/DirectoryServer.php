<?php  /*\
        *   Web Game List Script
        *
        *   This PHP program work with RakNet to maintain a list of
        *   currently running game servers.  The servers communicate
        *   call this script via HTTP periodically and submit thier
        *   updated information.
        *
        *   Requirements:
        *       (1) A place to write a file
        *       (2) PHP version 4.3.0 or newer (including PHP 5)
        *
        *   To install:
        *       (1) Place this file anywhere your web-server can access.
        *       (2) Make sure the configuration file can be written.
        *           If your script can't write to the current
        *           directory, this change the path CONF_FILE below.
        *       (3) Run the script via a web browser, for example like
        *           http://www.yourserver.com/directoryserver.php
        *       (4) Set a password and save the configuration.
        *
        *   Troubleshooting:
        *       We use the standard PHP logging mechanism, so check your
        *       web server error log if you encounter any problems.
        *
        *   History:
        *       2008 April 21 - Initial creation
        *       2008 May 1 - Update escape handling
       \*/

    header("Cache-Control: no-cache, must-revalidate");

    define("CONF_FILE",'./directoryserver.cfg');

    // Initialize configuration variables
    // (configuration includes the game database)
    $gv = array(
        'title' => 'directoryserver',
        'password' => '', // for changing the configuration
        'max_length' => 5000, // for a game, total # of field
        'max_timeout' => 5000, // timeouts are in seconds
        'games' => array() // game database, indexed by IP address
    );

    $config_message = "";

    $posted = strcmp($_SERVER["REQUEST_METHOD"],"POST") == 0;

    // Lock configuration if we may have updates
    if($posted)
        $lock_file = fopen(CONF_FILE . ".lock", "w")
        and flock($lock_file, LOCK_EX);

    if(! file_exists(CONF_FILE))
        trigger_error(
            "The configuration file \"" . CONF_FILE . "\"
             does not yet exist.  Try revealing the directoryserver configuration, setting the <em>title</em>,
             and a new <em>password</em> below, then submit the
             updated information. ");
    else
        include(CONF_FILE);

    // update handling
    if($posted) {
        $password_correct = false;

        if(isset($_GET['update']) || isset($_GET['query']))
            update_game_list();
        else
            update_config();

        // Remove stale servers (over 10X max_timeout expired)
        $ips = array_keys($gv['games']);
        foreach($ips as $ip) {
            if($gv['games'][$ip]['__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE'] +
               $gv['max_timeout'] * 10 < time())
                unset($gv['games'][$ip]);
        }


        // Save configuration variable ($gv) to file
        $new_cfg = fopen(CONF_FILE . ".tmp","w")
        and fwrite($new_cfg,"<?php \$gv = \n")
        and fwrite($new_cfg,var_export($gv,true))
        and fwrite($new_cfg,"?>")
        and fclose($new_cfg)
        and rename(CONF_FILE . ".tmp",CONF_FILE);

        fclose($lock_file);
    }

    // don't send HTML to a game server
    if(isset($_GET['update']) or isset($_GET['query']))
        {
        respond_to_game_server();
        exit(0);
        }
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "DTD/xhtml1-transitional.dtd">
<html>
<head>
    <title><?php echo $gv['title'] ?></title>
    <style type="text/css">

        /* Main body styles */
        body { font-family: "Verdana", sans-serif; margin: 0;
               padding: 0; background: #EEE; }
        h1 { line-height: 3em; font-size: xx-large; vertical-align: middle;
             background: #116; color: white; font-weight: bold;
             font-family: "Georgia", serif; margin: 0;
             border-bottom: thin solid white; }
        #config_msg { background: red; margin: 0; padding: 0.5em; }
        p { margin: 0.5em 1em; }

        /* Styling of game records */
        .games div { width: 35em; float: left; margin: 1em; background: #CCE;
                     border-width: 1px 2px 2px 1px; border-style: solid; }
        .games p span { display: block; clear: both; }
        .games p strong { font-size: larger; float: left; }
        .games p em { float: right; }
        .games ul { border-top: dashed thin grey; margin: 0.5em;
                    padding: 0.5em; text-align: center; }
        .games li { list-style-position: inside; }

        /* Styling of configuration box */
        form { clear: both; background: #AAF; margin: 1em 1em 0 1em;
               padding: 0.2em 0.5em; font-size: small;
               border: thin solid black; }
        form h2 { margin: 0; font-weight: normal; font-family: "Georgia", serif; }
        form #submit { width: 60%; display: block; margin: 0.1em auto 0.5em auto; }
        form.closed_form p { display: none; }
        form.closed_form #submit { display: none; }
    </style>
    <script type="text/javascript">
        function init() {
            var block =  document.getElementById("conf_form");

            block.getElementsByTagName("input")[0].onclick = function () {
                block.className = block.className == "" ? "closed_form" : "";
            }

            block.className = "closed_form";
        }
    </script>
</head>
<body onload="init()">
    <h1><?php echo $gv['title'] ?></h1>

    <?php if(! empty($config_message))
              echo "<p id='config_msg'>" . $config_message . "</p>\n\t";

        $myurl = $_SERVER['SCRIPT_NAME'];
        if(isset($_GET['showold']))
            $msg = "<a href='$myurl'>Show only functioning servers</a>";
        else
            $msg = "<a href='$myurl?showold'>Show all servers, including unresponsive ones</a>";
        $game_count = count($gv['games']);
        echo "<p>Known Servers: $game_count $msg</p>\n";
        ?>


    <div class="games">
        <?php foreach($gv['games'] as $address => $fields)
                    output_game($address, $fields);
        ?>
    </div>

    <form method="post" id="conf_form">
        <h2>directoryserver configuration <input type="button" value="Reveal / Hide" /></h2>
        <p>To make changes, supply the <label>current password:
            <input type="password" name="password" /></label>
            <em>(by default the password is blank)</em>.</p>

        <p>To create a new password type it twice, here:
            <input type="password" name="newpass1" /> and here:
            <input type="password" name="newpass2" />.</p>

        <p>Change the page title:
            <input name="title" value="<?php echo $gv['title'] ?>" /></p>

        <p>Change the maximum number of characters allowed in a record:
            <input name="max_length" value="<?php echo $gv['max_length'] ?>" />.
            Records longer than this will be truncated.</p>

        <p>Change the longest timeout value in seconds game servers can use:
            <input name="max_timeout" value="<?php echo $gv['max_timeout'] ?>" />.
            Records older than this will not be shown.</p>

        <input id="submit" type="submit" value="Submit Updated Configuration Information" />
    </form>
</body>
</html>
<?php // functions follow

function update_game_list() {
    global $gv;
    global $delete_record;
    global $password_correct;

    $port = 0;
    $updates = array();


    // apply updates
    while(list($akey, $avalue) = each($_POST)){

        if(0 == strcasecmp("__GAME_PORT",$akey)) {
            $port = $avalue;
            continue;
        } elseif(0 == strcasecmp("__DELETE_ROW",$akey)) {
            $delete_record = true;
        } elseif(0 == strcasecmp("__PHP_DIRECTORY_SERVER_PASSWORD",$akey)) {
            if(0 == strcmp($avalue, $gv['password']))

                $password_correct = true;
            continue;
        }

        $updates[$akey] = $avalue;

        if(strcasecmp("__GAME_LISTING_TIMEOUT",$akey) == 0 && $avalue > $gv['max_timeout'])
            trigger_error("Updated record timeout too long", E_USER_WARNING);
    }

    if(!$password_correct && !empty($gv['password'])) {
        trigger_error("Supplied password is wrong", E_USER_ERROR);
        return;
    }

    if(! isset($_GET['update']))
        return;

    if($delete_record) {
        unset($gv['games'][$_SERVER["REMOTE_ADDR"] . ":" . $port]);
        return;
    }

    $game =& $gv['games'][$_SERVER["REMOTE_ADDR"] . ":" . $port];

    if(strlen(implode(array_keys($game))) + strlen(implode($game))
           + strlen(array_keys($updates)) + strlen(array_values($updates)) > $gv['max_length']) {
        trigger_error("Dumping old record to make room for new information");
        $game = array();
    }

    foreach($updates as $akey => $avalue) {
        $game[$akey] = $avalue;
        if(empty($avalue))
            unset($game[$akey]);
    }

    $game['__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE'] = time();
}

function respond_to_game_server() {
    global $gv;
    global $delete_record;
    global $password_correct;

    echo "Configuration follows\n";
    foreach($gv as $key => $value) {
        if(strcmp($key,"games") == 0 or strcmp($key,"password") == 0)
            continue;
        echo $key . ": " . strtr($value, "\n\001\002", "\r  ") . "\n";
    }

    if($delete_record)
        echo "Entry Deleted";

    if(! isset($_GET['query']))
        return;

    if(!$password_correct && !empty($gv['password'])) {
        trigger_error("Supplied password is wrong", E_USER_ERROR);
        return;
    }

    echo "Game List follows\n";
    foreach($gv['games'] as $ip => $game) {
        if($game['__GAME_LISTING_TIMEOUT'] + $game['__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE'] < time())
            continue;
        foreach($game as $key => $value) {

            echo strtr($key, "\n\001\002", "\r  ") .
                "\002" . strtr($value, "\n\001\002", "\r  ") . "\001";
        }
        echo "__SystemAddress\002$ip\n";
    }

    echo "End of Response\n";
}

function update_config() {
    global $gv, $config_message;

    // check password
    if(strcmp($gv['password'],$_POST['password'])) {
        $config_message .= "Invalid password supplied\n";
        return;
    }

    if(! empty($_POST['max_length']))
        $gv['max_length'] = (int)$_POST['max_length'];
    if(! empty($_POST['max_timeout']))
        $gv['max_timeout'] = (int)$_POST['max_timeout'];
    if(! empty($_POST['title']))
        $gv['title'] = $_POST['title'];

    if(! empty($_POST['newpass1'])) {

        if(strcmp($_POST['newpass1'], $_POST['newpass2']))
            $config_message = "New passwords do not match.\n";
        else
            $gv['password'] = $_POST['newpass1'];
    }

    $config_message .= "Configuration updated.";
}

function output_game($address, $fields) {
    global $gv;

    // default field values
    $generics = "";
    $timeout = $gv['max_timeout'];
    $name = "Untitled";

    // assign special fields to a variable or add to generic list
    // (field names are case-insensitve)
    foreach($fields as $key => $value) {
        if(strcasecmp($key,"__GAME_NAME") == 0)
            $name = $value;
        elseif(strcasecmp($key,"__GAME_LISTING_TIMEOUT") == 0)
            $timeout = min($value, $timeout);
        elseif(strcasecmp($key,"__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE"))
            $generics .= "<li><strong>$key</strong>: $value</li>\n\t\t";
    }

    // when does this record expire?
    $update = date("D M j G:i:s T Y", $fields['__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE']);
    $expiration = $fields['__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE'] + $timeout;
    $remaining = $expiration - time();
    if(! isset($_GET['showold']) && $remaining < 0)
        return;

    // output record
    echo <<<DOC
        <div>
            <p>
                <strong>$name</strong>
                <em>$address</em>
                <span>last updated at $update<br />
                    (this record expires in $remaining seconds)</span>
            </p>
            <ul>
                $generics
            </ul>
        </div>
DOC;
}

?>
