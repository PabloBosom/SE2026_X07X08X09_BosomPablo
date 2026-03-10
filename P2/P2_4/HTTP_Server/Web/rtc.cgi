t <html>
t   <head>
t     <title>RTC Control</title>
t     <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t     <script language=JavaScript type="text/javascript">
#     Define URL and refresh timeout
t       var formUpdate_rtc = new periodicObj("rtc.cgx", 1000);
t       function periodicUpdateRTC() {
t         if(document.getElementById("RTCChkBox").checked == true) {
t           updateMultiple(formUpdate_rtc);
t           RTC_elTime = setTimeout(periodicUpdateRTC, formUpdate_rtc.period);
t         }
t         else
t           clearTimeout(RTC_elTime);
t         }
t     </script>
t   </head>
i   pg_header.inc
t     <h2 align="center"><br>Fecha y Hora</h2>
t     <p><font size="2">This page allows you to monitor Hour and Date values from RTC. 
t     Periodic screen update is based on <b>xml</b> technology.
t     This results in smooth flicker-free screen update.<br><br></font></p>
t     <form action="rtc.cgi" method="post" id="form1" name="form1">
t 		<p align=center>
t 		<input type=button value="Refresh" onclick="updateMultiple(formUpdate_rtc)">
t 		Periodic:<input type="checkbox" id="RTCChkBox" onclick="periodicUpdateRTC()">
t 	  </p></form>
t     <table border=0 width=99%><font size="3">
t         <tr style="background-color: #aaccff">
t         <th width=45%>Item</th>
t         <th width=35%>Value</th></tr>
t <tr><td><img src="pabb.gif">FECHA:</td>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c z 2  size="10" id="Date_Val" value="%s"></td></tr>
t <tr><td><img src="pabb.gif">HORA:</td>
t   <p align="center">
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c z 1  size="10" id="Time_Val" value="%s"></td></tr>
i pg_footer.inc
. End of script must be closed with period
