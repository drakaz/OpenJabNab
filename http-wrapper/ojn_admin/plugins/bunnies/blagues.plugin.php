<?php
if(!empty($_POST['frequency'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/blagues/setFrequency?value=".$_POST['frequency']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=blagues");
}
$frequency = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/blagues/getFrequency?".$ojnAPI->getToken());
$frequency = isset($frequency['ok']) ? $frequency['ok'] : '';
?>
<form method="post">
<fieldset>
<legend>Votre lapin aime les bonnes blagues...</legend>
<select name="frequency"> <option value="50" <?php if ($frequency==50) echo 'selected'; ?> >Un peu...</option><option value="125" <?php if ($frequency==125) echo 'selected'; ?>>Beaucoup...</option><option value="250" <?php if ($frequency==250) echo 'selected'; ?>>A la folie...</option><option value="1" <?php if ($frequency==-1) echo 'selected'; ?>>Pas du tout!</option></select>
<input type="submit" value="Enregistrer">
</fieldset>
</form>
