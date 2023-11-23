% Reading an image
image = imread('qtt6q1d6-720.jpg');


resizedImage = imresize(image, [48,72], 'bicubic');

imshow(resizedImage);


% Convert the image to a double array
noisyArray = double(resizedImage);

% Reshape the array to a 1D array for row-wise writing
noisyArray_reshaped = reshape(noisyArray, 1, []);

% Write the reshaped array to a text file in a row-wise format
fileID = fopen('noisy_imagearr.txt', 'w');
fprintf(fileID, '%d ', noisyArray_reshaped);
fclose(fileID);
