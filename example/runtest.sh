#!/usr/bin/env bash

# Test each Dilithium version
for VERSION in 2 3 5; do
    echo -e "\n🔰 Testing Dilithium${VERSION}"
    echo "================================"

    echo "🔑 Generating keys..."
    ./dilithium --keygen -v ${VERSION} -p public${VERSION}.key -s secret${VERSION}.key
    if [ $? -ne 0 ]; then
        echo "❌ Key generation failed for version ${VERSION}"
        exit 1
    fi
    echo "✅ Keys generated"

    echo -e "\n📝 Creating test message..."
    echo "Hello, World! Testing Dilithium${VERSION}" > message${VERSION}.txt
    echo "✅ Message created"

    echo -e "\n🖋️  Signing message..."
    ./dilithium --sign -v ${VERSION} -s secret${VERSION}.key -i message${VERSION}.txt -o signature${VERSION}.bin
    if [ $? -ne 0 ]; then
        echo "❌ Signing failed for version ${VERSION}"
        exit 1
    fi
    echo "✅ Signature created"

    echo -e "\n🔍 Verifying signature..."
    ./dilithium --verify -v ${VERSION} -p public${VERSION}.key -i message${VERSION}.txt -S signature${VERSION}.bin
    if [ $? -eq 0 ]; then
        echo "✅ Signature is valid!"
    else
        echo "❌ Signature verification failed!"
        exit 1
    fi

    echo -e "\n🧪 Testing negative case (modified message)..."
    echo "Modified message" > message${VERSION}_modified.txt
    ./dilithium --verify -v ${VERSION} -p public${VERSION}.key -i message${VERSION}_modified.txt -S signature${VERSION}.bin
    if [ $? -ne 0 ]; then
        echo "✅ Verification correctly failed for modified message"
    else
        echo "❌ Verification unexpectedly succeeded for modified message"
        exit 1
    fi

    echo -e "\n🧹 Cleaning up version ${VERSION} test files..."
    rm -f public${VERSION}.key secret${VERSION}.key message${VERSION}.txt message${VERSION}_modified.txt signature${VERSION}.bin
done

echo -e "\n🎉 All tests completed successfully!"
