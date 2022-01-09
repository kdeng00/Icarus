using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models
{
    [Table("User")]
    public class User
    {
        [JsonProperty("user_id")]
        [Column("UserID")]
        [Key]
        public int UserID { get; set; }
        [JsonProperty("username")]
        public string Username { get; set; }
        [JsonProperty("password")]
        public string Password { get; set; }
        [JsonProperty("email")]
        public string Email { get; set; }
        [JsonProperty("phone")]
        [Column("Phone")]
        public string Phone { get; set; }
        [JsonProperty("firstname")]
        public string Firstname { get; set; }
        [JsonProperty("lastname")]
        public string Lastname { get; set; }
        [JsonProperty("email_verified")]
        [NotMapped]
        public bool EmailVerified { get; set; }
        [JsonProperty("date_created")]
        public DateTime DateCreated { get; set; }
        [JsonProperty("status")]
        public string Status { get; set; }
        [JsonProperty("last_login")]
        public DateTime? LastLogin { get; set; }
    }
}
