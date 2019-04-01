using System;
using System.IO;
using System.IO.Compression;
using System.Threading.Tasks;


using SevenZip.Compression.LZMA;

namespace Icarus.Controllers.Utilities
{
    public class SongCompression
    {
        #region Fields
        byte[] _uncompressedSong;
        #endregion


        #region Propterties
        #endregion


        #region Constructors
        public SongCompression()
        {
        }
        public SongCompression(byte[] uncompressedSong)
        {
            _uncompressedSong = uncompressedSong;
        }
        #endregion


        #region Methods
        //public async Task<byte[]> CompressedSong(byte[] uncompressedSong)
        public async Task<byte[]> CompressedSong(byte[] uncompressedSong)
        {
            byte[] compressedSong = null;
            try
            {
                //compressedSong = SevenZip.Compression.LZMA.SevenZipHelper.Compress(uncompressedSong);
                MemoryStream output = new MemoryStream();
                using (DeflateStream dstream = new DeflateStream(output, CompressionLevel.Optimal))
                {
                    dstream.Write(uncompressedSong, 0, uncompressedSong.Length);
                    compressedSong = output.ToArray();
                }
            }
            catch (Exception ex)
            {
                var exMsg = ex.Message;
                Console.WriteLine("An error ocurred:");
                Console.WriteLine(exMsg);
            }

            return compressedSong;
        }
        #endregion
    }
}
