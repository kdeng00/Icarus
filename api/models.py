from django.db import models

class Song(models.Models):
    title = models.CharField(max_length=70)
    albumn = models.CharField(max_length=70)
    year = models.IntegerField()
    genre = models.CharField(max_length=70)
    track_number = models.IntegerField()
