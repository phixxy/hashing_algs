message = input()

BIT_32_MASK = 0xFFFFFFFF

def convert_msg_to_bytes(message):
   byte_msg = []
   for letter in message:
      b = ord(letter)
      byte_msg.append(b)
   return byte_msg

def right_rotate(word, amount):
    amount = amount % 32
    return ((word >> amount) | (word << (32 - amount))) & 0xFFFFFFFF


# init hash values
h0 = 0x6a09e667
h1 = 0xbb67ae85
h2 = 0x3c6ef372
h3 = 0xa54ff53a
h4 = 0x510e527f
h5 = 0x9b05688c
h6 = 0x1f83d9ab
h7 = 0x5be0cd19

# round of constants
k = [0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2]

# pre-processing
byte_msg = convert_msg_to_bytes(message)
L = len(byte_msg) * 8

# append 1 bit to message (actually 8 bits)
byte_msg.append(128) #appends 10000000, which is always safe

# append K 0 bits to message where K is (L + 8 + K + 64) is lowest multiple of 512
size = L + 8 + 64
new_len = 512
while new_len <= size:
   new_len += 512
byte_msg.extend([0] * ((new_len - size)//8))

# append L as a 64 bit big-endian int making the total post processed len a mult of 512 bits
L_bytes = L.to_bytes(8, byteorder='big')
byte_msg.extend(L_bytes)
#<original message of len L> 1 <K zeros> <L as 64 bit int>, (the number of bits will be a mult of 512)

# break into 512 bit chunks
chunks = []
for x in range(0,int(new_len/512)):
   tmp = byte_msg[x*64:(x+1)*64]
   word = []
   tmp_word = 0
   for y in range(0,int(256/4)):
      for item in tmp[y*4:(y+1)*4]:
         tmp_word = (tmp_word << 8) | item
      word.append(tmp_word)
      tmp_word = 0
   chunks.append(word)

# process words
for w in chunks:
   for i in range(16,64):
      s0 = ((right_rotate(w[i-15],7) ^ right_rotate(w[i-15], 18) ^ w[i-15] >> 3)) & BIT_32_MASK
      s1 = ((right_rotate(w[i-2], 17) ^ right_rotate(w[i-2], 19) ^ w[i-2] >> 10)) & BIT_32_MASK
      w[i] = (w[i-16] + s0 + w[i-7] + s1) & BIT_32_MASK


   # compression time!
   a = h0
   b = h1
   c = h2
   d = h3
   e = h4
   f = h5
   g = h6
   h = h7
   for i in range(0,64):
      s1 = right_rotate(e,6) ^ right_rotate(e,11) ^ right_rotate(e,25) & BIT_32_MASK
      ch = ((e & f) ^ ((~e) & g)) & BIT_32_MASK
      temp1 = (h + s1 + ch + k[i] + w[i]) & BIT_32_MASK
      s0 = (right_rotate(a,2) ^ right_rotate(a,13) ^ right_rotate(a,22)) & BIT_32_MASK
      maj = ((a & b) ^ (a & c) ^ (b & c)) & BIT_32_MASK
      temp2 = (s0 + maj) & BIT_32_MASK
      h = g
      g = f
      f = e
      e = (d + temp1) & BIT_32_MASK
      d = c
      c = b
      b = a
      a = (temp1 + temp2) & BIT_32_MASK

   # add compressed chunk
   h0 = (h0 + a) & BIT_32_MASK
   h1 = (h1 + b) & BIT_32_MASK
   h2 = (h2 + c) & BIT_32_MASK
   h3 = (h3 + d) & BIT_32_MASK
   h4 = (h4 + e) & BIT_32_MASK
   h5 = (h5 + f) & BIT_32_MASK
   h6 = (h6 + g) & BIT_32_MASK
   h7 = (h7 + h) & BIT_32_MASK

# create digest
digest = 0
for i in [h0, h1, h2, h3, h4, h5, h6, h7]:
   digest = (digest << 32) | i
print(hex(digest)[2:].zfill(64))