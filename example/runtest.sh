#! /usr/bin/env bash

echo "🔑 Generating keys..."
# Generate both secret and public keys at once
./generate_secretkey -p public.key -s secret.key
echo "✅ Keys generated"

echo -e "\n📝 Creating test message..."
echo "Hello, World!" > message.txt
echo "✅ Message created"

echo -e "\n🖋️  Signing message..."
./create_signature -s secret.key -p public.key -i message.txt > signature.bin
echo "✅ Signature created"

echo -e "\n🔍 Verifying signature..."
./validate_signature -p public.key -i message.txt -s signature.bin 2> /dev/null
if [ $? -eq 0 ]; then
    echo "✅ Signature is valid!"
else
    echo "❌ Signature verification failed!"
fi
