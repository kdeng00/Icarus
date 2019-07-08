using System;
using System.Collections.Generic;

using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Database.Repositories
{
    public class CoverArtRepository : BaseRepository
    {
        #region Constructors
        public CoverArtRepository(string connectionString)
        {
            _connectionString = connectionString;
        }
        #endregion


        #region Methods
        public CoverArt GetCoverArt(CoverArt coverArt)
        {
            // TODO: Implement sql record retrieval
            //
            try
            {
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
            }
            return null;
        }
        public CoverArt GetCoverArt(Song song)
        {
            // TODO: Implement sql record retrieval
            try
            {
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
            }
            return null;
        }

        public void SaveCoverArt(CoverArt coverArt)
        {
            try
            {
                using (var conn = GetConnection())
                {
                    conn.Open();

                    var query = "INSERT INTO CoverArt(SongTitle, ImagePath) VALUES(@SongTitle, " +
                        "@ImagePath)";
                    using (var cmd = new MySqlCommand(query, conn))
                    {
                        cmd.Parameters.AddWithValue("@SongTitle", coverArt.SongTitleA);
                        cmd.Parameters.AddWithValue("@ImagePath", coverArt.ImagePath);

                        cmd.ExecuteNonQuery();
                    }
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
            }
        }
        public void DeleteCoverArt(CoverArt coverArt)
        {
            try
            {
                // TODO: Implement sql record deletion
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
        }

        private List<CoverArt> ParseData(MySqlDataReader reader)
        {
            if (reader.HasRows)
            {
                var coverArtList = new List<CoverArt>();
                while (reader.Read())
                    coverArtList.Add(new CoverArt
                    {
                        CoverArtId = Convert.ToInt32(reader["CoverArtId"]),
                        SongTitle = reader["SongTitle"].ToString(),
                        ImagePath = reader["ImagePath"].ToString()
                    });

                return coverArtList;
            }

            return null;
        }
    
        private CoverArt ParseSingleData(MySqlDataReader reader)
        {
            if (reader.HasRows)
            {
                reader.Read();

                return new CoverArt
                {
                    CoverArtId = Convert.ToInt32(reader["CoverArtId"]),
                    SongTitle = reader["SongTitle"].ToString(),
                    ImagePath = reader["ImagePath}"].ToString()
                };
            }

            return null;
        }

        private bool? AnyCoverArt()
        {
            using (var conn = GetConnection())
            {
                conn.Open();

                var query = "SELECT * FROM CoverArt";
                using (var cmd = new MySqlCommand(query, conn))
                    using (var reader = cmd.ExecuteReader())
                        return reader.HasRows;
            }
        }
        #endregion
    }
}
