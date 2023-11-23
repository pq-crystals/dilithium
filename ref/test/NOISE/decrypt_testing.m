% Replace these values with the actual dimensions of your image
imageWidth = 48;
imageHeight = 72;
numChannels = 3;  % Assuming RGB

% Read the 1D array from the text file
data = load('decrypt.txt');

% Reshape the 1D array into a 3D matrix
imageMatrix = reshape(data, imageWidth, imageHeight, numChannels);

% Normalize the image data to the range [0, 1]
imageMatrix = double(imageMatrix) / 255.0;

% Display the image
imshow(imageMatrix);
title('Your Image');
