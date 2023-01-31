<?php
    // define PDD - tell about database file

    $pdo = new PDO('sqlite:DB_HealthRemoteMonitorSystem.db');


    //write SQL
    $statement = $pdo->query("SELECT * FROM LOCATION");

    //run SQL
    $rows = $statement->fetchAll(PDO::FETCH_ASSOC);
    
    $stackLAT = array();
    $stackLON = array();   
    $stackTSLoc = array();

    //show it on the screen
    //print_r($rows);
    foreach($rows as $row)
    {
    //print "<td>".$row['BPM']."</td>";
    array_push($stackLAT,$row['LAT']);
    array_push($stackLON,$row['LON']);

    //print "<td>".$stackLON[0]."</td>";
    array_push($stackTSLoc,$row['TIMESTAMP']);
   // print "<td>".$row['TIMESTAMP']."</td>";
    //print "<p>";
    }
    $lat = end($stackLAT);$comma=",";$lat .= $comma;
    $lon = end($stackLON);
    $latlong = $lat . $lon;
    
    $link1 = "https://www.google.com/maps/embed/v1/place?key=AIzaSyD86WNILiswgc7JeKL7WAbwvZb5piIxQwE&q=";
    $link1 .= $latlong;
    $link1 .="&zoom=14";


?>
<style>
  .wrapper {
    text-align: center;
}

.button {
    background-color: #000; /* Green */
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
}
</style>

<!DOCTYPE html>
<html>
<iframe
  width="600"
  height="450"
  style="border:0"
  loading="lazy"
  allowfullscreen
  referrerpolicy="no-referrer-when-downgrade"
  src="<?php echo $link1; ?>"
  >
</iframe>
  
<div class="wrapper">
<button onclick="location.href='http://localhost:1234/'" type="button"  class="button">
         Return to App Menu</button>
         </div>

</html>