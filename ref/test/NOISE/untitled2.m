% Reading an image
image1 = imread('qtt6q1d6-720.jpg');

% Displaying an image
imshow(image1);

% Adding salt & pepper noise to the image
SPI = imnoise(image1, 'salt & pepper', 0.3);
imshow(SPI);

resizedImage = imresize(SPI, [48,72], 'bicubic');

% Convert the image to a double array
noisyArray1 = double(resizedImage);

% Reshape the array to a 1D array for row-wise writing
noisyArray_reshaped1 = reshape(noisyArray1, 1, []);

% Write the reshaped array to a text file in a row-wise format
fileID = fopen('sample2.txt', 'w');
fprintf(fileID, '%d ', noisyArray_reshaped1);
fclose(fileID);
