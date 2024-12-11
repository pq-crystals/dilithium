#! /usr/bin/env bash

# Generate a secret key
./generate_secretkey > secret.key

# Generate the corresponding public key
cat secret.key | ./secretkey_to_publickey > public.key

# Create a signature for a message
echo "Hello, World!" > message.txt
./create_signature -k secret.key -i message.txt > signature.bin

# Verify the signature
./validate_signature -p public.key -i message.txt -s signature.bin
echo $?  # Should print 0 if signature is valid