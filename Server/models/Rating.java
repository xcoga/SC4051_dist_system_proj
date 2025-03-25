package Server.models;

public class Rating {
    private double totalRatingSum;
    private int numberOfRatings;
    private double averageRating;

    public Rating() {
        this.totalRatingSum = 0.0;
        this.numberOfRatings = 0;
        this.averageRating = 0.0;
    }

    public void addRating(double rating) {
        this.totalRatingSum += rating;
        this.numberOfRatings++;
        calculateAverageRating();
    }

    private void calculateAverageRating() {
        if (numberOfRatings == 0) {
            this.averageRating = 0.0;
            return;
        }
        
        this.averageRating = this.totalRatingSum / this.numberOfRatings;
    }

    public double getAverageRating() {
        return this.averageRating;
    }

    public int getNumberOfRatings() {
        return this.numberOfRatings;
    }

    public double getTotalRatingSum() {
        return this.totalRatingSum;
    }

    @Override
    public String toString() {
        return "Rating: " + this.averageRating + " (" + this.numberOfRatings + " ratings)";
    }
}