<?php 
if(!empty($_POST['a']) && (isset($_POST['tvtime']) || isset($_POST['tvchan']))) {
	if($_POST['a']=="addwbc" && !empty($_POST['tvtime']))
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/tv/addwebcast?time=".$_POST['tvtime']."&".$ojnAPI->getToken());
	elseif($_POST['a']=="setchan") {
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/tv/setchannel?list=".urlencode(implode(",", $_POST['tvchan']))."&".$ojnAPI->getToken());
	}
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=tv");
}
else if(!empty($_GET['rmwbc'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/tv/removewebcast?time=".$_GET['rmwbc']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=tv");
} 
$chans = array("TF1", "France 2", "France 3", "Canal+", "France 5", "M6", "Arte", "D8", "W9", "TMC", "NT1", "NRJ 12", "La Chaîne Parlementaire / Public Sénat", "France 4", "BFM TV", "i>Télé", "D17", "Gulli", "France Ô", "HD1", "L'Equipe 21", "6ter", "Numéro 23", "RMC Découverte", "Chérie 25", "LCI - La Chaîne Info", "Paris Première", "Canal+ sport", "Canal+ cinéma", "Planète +", "Eurosport", "Extra : include late night program for selection" );
$channels = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/tv/listchannel?".$ojnAPI->getToken());
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="addwbc" checked="true" /> Add Webcast<br />
<input type="radio" name="a" value="setchan" /> Set channels<br /><br />
Time:<input type="text" name="tvtime" /><br />
Channels:<br />
<?php foreach ($chans as $i => $chan) {
?>
<input type="checkbox" name="tvchan[]" value="<?php echo $chan; ?>" <?php if(in_array($chan, $channels)) { ?> checked="checked"<?php } ?>><?php echo $chan; ?><?php if(($i+1) % 4 == 0) { ?><br /><?php } ?> 
<?php } ?>
<br />
<input type="submit" value="Enregister">

<?php
$webcasts = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/tv/listwebcast?".$ojnAPI->getToken());
if($webcasts){
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="3">Webcast</th>
	</tr>
	<tr>
		<th>Time</th>
		<th>Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($webcasts as $item) {
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo urldecode($item) ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=tv&rmwbc=<?php echo $item ?>">Remove</a></td>
	</tr>
<?php  } ?>
</table>
<?php } ?>
</fieldset>
</form>
