from django.db import models

class Song(models.Model):
    title = models.CharField(max_length=70)
    album = models.CharField(max_length=170)
    artist = models.CharField(max_length=100)
    year = models.IntegerField()
    genre = models.CharField(max_length=70)
    duration = models.IntegerField()
    track_number = models.IntegerField()
    cover = models.BinaryField(editable=True)
    file = models.BinaryField(editable=True)
