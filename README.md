This is helpful for Elderly patients.
by measuring heart rate and valuate body temperature can  warning  via LINE Notify when these values are abnormal.
More over, this tool have the database sytem of these values with google spreadsheet.  

google script:
function doPost(e) {
  try {
    if (e === undefined) {
      throw new Error("No POST data received.");
    }
    var sheet = SpreadsheetApp.openById(" YOUR SPREADSHEET ID ").getActiveSheet();
    var data = JSON.parse(e.postData.contents);
    
    var newRow = [
      new Date(),
      data.sensor1,
      data.sensor2
    ];

    sheet.appendRow(newRow);
    return ContentService.createTextOutput("Success");
  } catch (error) {
    Logger.log("Error: " + error.message);
    return ContentService.createTextOutput("Error: " + error.message);
  }
}
