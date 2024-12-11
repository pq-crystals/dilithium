{
  description = "Dilithium post-quantum cryptography implementation";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        dilithium = pkgs.stdenv.mkDerivation {
            name = "dilithium";
            src = ./.;

            nativeBuildInputs = with pkgs; [
              clang
              openssl
            ];

            buildPhase = ''
              # Save the current directory to buildDir
              buildDir=$PWD

              # Build reference implementation with shared libraries
              cd ref
              make -j$NIX_BUILD_CORES
              make shared
              
              # Build all tests for reference implementation
              for alg in 2 3 5; do
                make test/test_dilithium$alg
                make test/test_vectors$alg
              done
              
              # Build AVX2 implementation if supported
              if [ "$(uname -m)" = "x86_64" ]; then
                cd ../avx2
                make -j$NIX_BUILD_CORES
                make shared
                
                # Build all tests for AVX2 implementation
                for alg in 2 3 5; do
                  make test/test_dilithium$alg
                  make test/test_vectors$alg
                done
              fi
            '';

            installPhase = ''
              mkdir -p $out/lib $out/include/dilithium
              
              cp -r $src/ref/*.h $out/include/dilithium/
              cp -r $buildDir/ref/libpqcrystals_*_ref.so $out/lib/
              
              # Install AVX2 implementation libraries if built
              if [ "$(uname -m)" = "x86_64" ]; then
                cp -r $buildDir/avx2/libpqcrystals_*_avx2.so $out/lib/
              fi
            '';

            checkPhase = ''
              mkdir -p $TMPDIR/test-outputs
              
              # Run tests
              for alg in 2 3 5; do
                ./test/test_dilithium$alg
                ./test/test_vectors$alg > $TMPDIR/test-outputs/tvecs$alg
              done

              # Verify checksums from temporary directory
              cd $TMPDIR/test-outputs
              sha256sum -c $src/SHA256SUMS
            '';

            doCheck = false;
          };
      in
      {
        packages = {
          inherit dilithium;
          default = dilithium;

          examples = pkgs.stdenv.mkDerivation {
            name = "dilithium-examples";
            src = ./.;
            propagatedBuildInputs = [ dilithium ];

            nativeBuildInputs = with pkgs; [
              gcc
              openssl
            ];

            buildPhase = ''
              export DILITHIUM_INCLUDE_DIR="${dilithium}/include"
              export DILITHIUM_LIB_DIR="${dilithium}/lib"
              export LD_LIBRARY_PATH="${dilithium}/lib:$LD_LIBRARY_PATH"

              cd example
              make -j$NIX_BUILD_CORES
            '';

            installPhase = ''
              mkdir -p $out/bin $out/lib
              
              # Install example binaries
              cp generate_secretkey $out/bin/
              cp secretkey_to_publickey $out/bin/
              cp create_signature $out/bin/
              cp validate_signature $out/bin/

              # Create wrapper scripts that set LD_LIBRARY_PATH
              for bin in generate_secretkey secretkey_to_publickey create_signature validate_signature; do
                mv $out/bin/$bin $out/bin/$bin.real
                cat > $out/bin/$bin <<EOF
#!${pkgs.bash}/bin/bash
export LD_LIBRARY_PATH=$out/lib:\$LD_LIBRARY_PATH
exec $out/bin/$bin.real "\$@"
EOF
                chmod +x $out/bin/$bin
              done
            '';
          };
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            clang
            openssl
            dilithium
          ];
        };
      }
    );
} 