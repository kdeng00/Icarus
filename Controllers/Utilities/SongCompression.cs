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
        public byte[] CompressedSong(byte[] uncompressedSong)
        {
            byte[] compressedSong = null;
            try
            {
				/**
                MemoryStream output = new MemoryStream();
                using (DeflateStream dstream = new DeflateStream(output, CompressionLevel.Optimal))
                {
                    dstream.Write(uncompressedSong, 0, uncompressedSong.Length);
                    compressedSong = output.ToArray();
                }
				*/

				// TODO: Implement song compression

				FileStream sourceFile = File.ReadAllBytes(uncompressedSong);
    			FileStream destinationFile = File.Create(path + ".gz");

			    byte[] buffer = new byte[sourceFile.Length];
    			sourceFile.Read(uncompressedSong, 0, uncompressedSong.Length);

    			using (GZipStream output = new GZipStream(destinationFile,
        			   CompressionMode.Compress))
    			{
       				 Console.WriteLine("Compressing {0} to {1}.", sourceFile.Name,
            							destinationFile.Name, false);

        			output.Write(buffer, 0, buffer.Length);
    			}

    			sourceFile.Close();
    			destinationFile.Close();

				Console.WriteLine("Song has been successfully compressed");
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
