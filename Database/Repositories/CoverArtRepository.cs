using System;
using System.Collections.Generic;

using MySql.Data.MySqlClient;

using Icarus.Models;
using Icarus.Types;

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
        public CoverArt GetCoverArt(CoverArt cover)
        {
            try
            {
                using (var conn = GetConnection())
                {
                    conn.Open();

                    _logger.Info("Querying cover art record");

                    var query = "SELECT * FROM CoverArt WHERE " +
                        "CoverArtId=@CoverArtId";
                    using (var cmd = new MySqlCommand(query, conn))
                    {
                        cmd.Parameters
                           .AddWithValue("@CoverArtId", cover.CoverArtId);

                        using (var reader = cmd.ExecuteReader())
                            return ParseSingleData(reader);
                    }
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }

            return null;
        }
        public CoverArt GetCoverArt(Song song)
        {
            try
            {
                using (var conn = GetConnection())
                {
                    conn.Open();

                    _logger.Info("Querying cover art record");

                    var query = "SELECT cov.* FROM CoverArt cov LEFT JOIN " +
                        "Song sng ON cov.CoverArtId=sng.CoverArtId WHERE " +
                        "sng.Id=@SongId";
                    using (var cmd = new MySqlCommand(query, conn))
                    {
                        cmd.Parameters.AddWithValue("@SongId", song.Id);

                        using (var reader = cmd.ExecuteReader())
                            return ParseSingleData(reader);
                    }
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }

            return null;
        }
        public CoverArt GetCoverArt(CoverArtField field, CoverArt cover)
        {
            try
            {
                using (var conn = GetConnection())
                {
                    conn.Open();

                    _logger.Info("Querying cover art record");

                    using (var cmd = new MySqlCommand(BuildQuery(field), conn))
                    {
                        switch (field)
                        {
                            case CoverArtField.SongTitle:
                                cmd.Parameters.AddWithValue("@SongTitle",
                                        cover.SongTitle);
                                break;
                            case CoverArtField.ImagePath:
                                cmd.Parameters.AddWithValue("@ImagePath", 
                                        cover.ImagePath);
                                break;
                        }

                        using (var reader = cmd.ExecuteReader())
                            return ParseSingleData(reader);
                    }
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }

            return null;
        }

        public bool DoesCoverArtExist(CoverArt cover)
        {
            return GetCoverArt(cover) != null ? true : false;
        }
        public bool DoesCoverArtExist(Song song)
        {
            return GetCoverArt(song) != null ? true : false;
        }

        public void SaveCoverArt(CoverArt coverArt)
        {
            try
            {
                using (var conn = GetConnection())
                {
                    conn.Open();

                    _logger.Info("Saving cover art record");

                    var query = "INSERT INTO CoverArt(SongTitle, ImagePath) " +
                        "VALUES(@SongTitle, @ImagePath)";
                    using (var cmd = new MySqlCommand(query, conn))
                    {
                        cmd.Parameters
                            .AddWithValue("@SongTitle", coverArt.SongTitle);
                        cmd.Parameters
                            .AddWithValue("@ImagePath", coverArt.ImagePath);

                        cmd.ExecuteNonQuery();
                    }
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
        }
        public void DeleteCoverArt(CoverArt cover)
        {
            try
            {
                using (var conn = GetConnection())
                {
                    conn.Open();

                    _logger.Info("Deleting cover art record");

                    var query = "DELETE FROM CoverArt WHERE " +
                        "CoverArtId=@CoverArtId";
                    using (var cmd = new MySqlCommand(query, conn))
                    {
                        cmd.Parameters
                            .AddWithValue("@CoverArtId", cover.CoverArtId);

                        cmd.ExecuteNonQuery();
                    }
                }
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
                _logger.Info("Parsing cover art records");
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
                _logger.Info("Parsing single cover art record");
                reader.Read();

                return new CoverArt
                {
                    CoverArtId = Convert.ToInt32(reader["CoverArtId"]),
                    SongTitle = reader["SongTitle"].ToString(),
                    ImagePath = reader["ImagePath"].ToString()
                };
            }

            return null;
        }

        private string BuildQuery(CoverArtField field)
        {
            switch (field)
            {
                case CoverArtField.SongTitle:
                    return "SELECT * FROM CoverArt WHERE SongTitle=@SongTitle";
                case CoverArtField.ImagePath:
                    return "SELECT * FROM CoverArt WHERE ImagePath=" +
                        "@ImagePath";
            }

            return null;
        }

        private bool? AnyCoverArt()
        {
            using (var conn = GetConnection())
            {
                conn.Open();

                _logger.Info("Checking to see if there are any cover art " + 
                        "records");

                var query = "SELECT * FROM CoverArt";
                using (var cmd = new MySqlCommand(query, conn))
                    using (var reader = cmd.ExecuteReader())
                        return reader.HasRows;
            }
        }
        #endregion
    }
}
