using System;
using System.Security.Cryptography;

using Microsoft.IdentityModel.Tokens;
using Org.BouncyCastle.Crypto.Parameters;
using Org.BouncyCastle.OpenSsl;


namespace Icarus.Certs;

public class SigningIssuerCertificate : IDisposable
{
    private readonly RSACryptoServiceProvider _rsa;

    public SigningIssuerCertificate()
    {
        _rsa = new RSACryptoServiceProvider();
    }

    public RsaSecurityKey GetIssuerSigningKey(string publicKeyPath)
    {
        var file = publicKeyPath;
        var publicKey = System.IO.File.ReadAllText(file);

        using (var reader = System.IO.File.OpenText(file))
        {
            var pem = new PemReader(reader);
            var o = (RsaKeyParameters)pem.ReadObject();
            var parameters = new RSAParameters();
            parameters.Modulus = o.Modulus.ToByteArray();
            parameters.Exponent = o.Exponent.ToByteArray();
            _rsa.ImportParameters(parameters);
        }

        return new RsaSecurityKey(_rsa);
    }


    public void Dispose()
    {
        _rsa?.Dispose();
    }
}


public class SigningAudienceCertificate : IDisposable
{
    private readonly RSACryptoServiceProvider _rsa;

    public SigningAudienceCertificate()
    {
        _rsa = new RSACryptoServiceProvider();
    }

    public SigningCredentials GetAudienceSigningKey(string keyPath)
    {
        var file = keyPath;
        var publicKey = System.IO.File.ReadAllText(file);

        using (var reader = System.IO.File.OpenText(file))
        {
            var pem = new PemReader(reader);
            var o = (RsaKeyParameters)pem.ReadObject();
            var parameters = new RSAParameters();
            parameters.Modulus = o.Modulus.ToByteArray();
            parameters.Exponent = o.Exponent.ToByteArray();
            _rsa.ImportParameters(parameters);
        }

        return new SigningCredentials(
            key: new RsaSecurityKey(_rsa),
            algorithm: SecurityAlgorithms.RsaSha256);
    }

    public void Dispose()
    {
        _rsa?.Dispose();
    }
}
