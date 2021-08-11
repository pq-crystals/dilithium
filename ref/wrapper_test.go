// Copyright (C) 2019-2021 Algorand, Inc.
// This file is part of go-algorand
//
// go-algorand is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// go-algorand is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with go-algorand.  If not, see <https://www.gnu.org/licenses/>.

package cdilithium

import (
	"crypto/sha256"
	"encoding/binary"
	"testing"

	"github.com/stretchr/testify/require"
)

func TestDilSigning(t *testing.T) {
	a := require.New(t)
	for i := 0; i < 100; i++ {
		sk, pk := NewKeys()
		b := make([]byte, 8)
		binary.BigEndian.PutUint64(b, uint64(i))
		bs := sha256.Sum256(b)
		sig := sk.SignBytes(bs[:])
		a.NoError(pk.VerifyBytes(bs[:], sig))
		var sig2 DilSignature
		copy(sig2[:], sig)

		sig2[0]++
		a.Error(pk.VerifyBytes(bs[:], sig2[:]))

		var bs2 [32]byte
		copy(bs2[:], bs[:])

		bs2[0]++
		a.Error(pk.VerifyBytes(bs2[:], sig[:]))
	}
}

func TestWrongSizedBytes(t *testing.T) {
	a := require.New(t)
	sk, pk := NewKeys()
	bs := sha256.Sum256(make([]byte, 8))
	sig := sk.SignBytes(bs[:])

	sig = append(sig, 0)
	a.Error(pk.VerifyBytes(bs[:], sig))

	sig = sig[:len(sig)-1]
	a.NoError(pk.VerifyBytes(bs[:], sig))

	sig = sig[:len(sig)-1]
	a.Error(pk.VerifyBytes(bs[:], sig))
}

func TestEmpty(t *testing.T) {
	a := require.New(t)
	sk, pk := NewKeys()
	bs := make([]byte, 0)
	sig := sk.SignBytes(bs)

	sig = append(sig, 0)
	a.Error(pk.VerifyBytes(bs[:], sig))

	sig = sig[:len(sig)-1]
	a.NoError(pk.VerifyBytes(bs[:], sig))

	sig = sig[:len(sig)-1]
	a.Error(pk.VerifyBytes(bs[:], sig))

	a.Error(pk.VerifyBytes(bs[:], nil))
}
