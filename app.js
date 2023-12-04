const express = require("express");
const bodyParser = require("body-parser");
const fs = require("fs");
const path = require("path"); // Node.js path module for working with file paths
const app = express();

// Middleware to parse JSON body
app.use(bodyParser.json());
// get method to deliver simple text to the browser
app.get("/", (req, res) => {
  res.send("Hello From Zishan!");
});
// Endpoint to handle incoming sensor data
app.post("/", (req, res) => {
  const tempData = req.body; // Assuming JSON data contains temperature and humidity

  const dataToSave = { data:`tempData`, timestamp: new Date() };
  const directoryPath = path.join(__dirname, "public"); // Path to your public folder

  // Ensure the 'public' directory exists; if not, create it
  if (!fs.existsSync(directoryPath)) {
    fs.mkdirSync(directoryPath);
  }

  // Save data to a new file (you may want to use a better file naming convention)
  const fileName = `sensor_data_${Date.now()}.json`;
  const filePath = path.join(directoryPath, fileName);

  fs.writeFile(filePath, JSON.stringify(dataToSave), (err) => {
    if (err) {
      console.error("Error saving file:", err);
      res.status(500).send("Error saving data");
    } else {
      console.log("Data saved successfully");
      res.status(200).send("Data saved");
    }
  });
});

// Endpoint to get information about files in the 'public' folder
app.get("/files-info", (req, res) => {
  const directoryPath = path.join(__dirname, "public"); // Path to your public folder

  fs.readdir(directoryPath, (err, files) => {
    if (err) {
      console.error("Error reading directory:", err);
      res.status(500).send("Error reading directory");
    } else {
      const filesInfo = [];
      files.forEach((file) => {
        const filePath = path.join(directoryPath, file);
        const stats = fs.statSync(filePath);
        filesInfo.push({
          fileName: file,
          sizeInBytes: stats.size,
          createdAt: stats.birthtime,

          // You can use stats.mtime for modification time
        });
      });
      res.status(200).json({
        message: "success",
        file_number: files.length,
       
          filesInfo,
        
      });
    }
  });
});

module.exports = app;
